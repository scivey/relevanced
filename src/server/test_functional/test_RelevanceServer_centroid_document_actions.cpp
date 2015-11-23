#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <memory>

#include <glog/logging.h>

#include <folly/ExceptionWrapper.h>
#include <folly/Memory.h>
#include <folly/futures/Try.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/Optional.h>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "testing/TestHelpers.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "persistence/InMemoryRockHandle.h"
#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "centroid_update_worker/DocumentAccumulatorFactory.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/Utf8Stemmer.h"
#include "stemmer/ThreadSafeStemmerManager.h"
#include "server/RelevanceServer.h"
#include "models/ProcessedDocument.h"
#include "server/RelevanceServer.h"
#include "models/Centroid.h"
#include "models/Document.h"

#include "util/util.h"
#include "text_util/ScoredWord.h"
#include "util/Clock.h"
#include "util/Sha1Hasher.h"
#include "server/test_functional/RelevanceServerTestCtx.h"

using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;
using namespace relevanced::util;
using namespace relevanced::text_util;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::document_processing_worker;
using namespace relevanced::similarity_score_worker;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::server;
using namespace relevanced::thrift_protocol;

using ::testing::Return;
using ::testing::_;

TEST(RelevanceServer, TestAddDocumentToCentroidHappy) {
  RelevanceServerTestCtx ctx;
  ctx.updateWorker->debug_getUpdateQueue()->debug_setShortTimeouts();
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request = folly::make_unique<AddDocumentsToCentroidRequest>();
  request->centroidId = "centroid-id";
  request->documentIds = vector<string> {"doc-id"};
  auto response = ctx.server->addDocumentsToCentroid(std::move(request)).get();
  EXPECT_FALSE(response.hasException());
  auto joinResponse = ctx.server->joinCentroid(folly::make_unique<string>("centroid-id"), false).get();
  EXPECT_FALSE(joinResponse.hasException());
  EXPECT_TRUE(joinResponse.value());
}

TEST(RelevanceServer, TestAddDocumentToCentroidAlreadyInCentroid) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request1 = folly::make_unique<AddDocumentsToCentroidRequest>();
  request1->centroidId = "centroid-id";
  request1->documentIds = vector<string> {"doc-id"};
  auto response1 = ctx.server->addDocumentsToCentroid(
    std::move(request1)
  ).get();
  EXPECT_FALSE(response1.hasException());
  auto request2 = folly::make_unique<AddDocumentsToCentroidRequest>();
  request2->centroidId = "centroid-id";
  request2->documentIds = vector<string> {"doc-id"};
  auto response2 = ctx.server->addDocumentsToCentroid(
    std::move(request2)
  ).get();
  EXPECT_TRUE(response2.hasException<EDocumentAlreadyInCentroid>());
}

TEST(RelevanceServer, TestAddDocumentToCentroidMissingDocument) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("unrelated-doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request = folly::make_unique<AddDocumentsToCentroidRequest>();
  request->centroidId = "centroid-id";
  request->documentIds = vector<string> {"missing-doc-id"};
  auto response = ctx.server->addDocumentsToCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<EDocumentDoesNotExist>());
}

TEST(RelevanceServer, TestAddDocumentToCentroidMissingCentroid) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request = folly::make_unique<AddDocumentsToCentroidRequest>();
  request->centroidId = "missing-centroid-id";
  request->documentIds = vector<string> {"doc-id"};
  auto response = ctx.server->addDocumentsToCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<ECentroidDoesNotExist>());
}

TEST(RelevanceServer, TestAddDocumentsToCentroidMissingBoth) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request = folly::make_unique<AddDocumentsToCentroidRequest>();
  request->centroidId = "missing-centroid-id";
  request->documentIds = vector<string> {"missing-doc-id"};
  auto response = ctx.server->addDocumentsToCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<ECentroidDoesNotExist>());
}

TEST(RelevanceServer, TestRemoveDocumentsFromCentroidHappy) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  vector<Future<Try<bool>>> saves;
  saves.push_back(
    ctx.persistence->saveCentroid("centroid-id", centroid)
  );
  saves.push_back(
    ctx.server->createDocumentWithID(
      folly::make_unique<string>("doc-id"),
      folly::make_unique<string>("some text about dogs"),
      Language::EN
    ).then([](Try<unique_ptr<string>> result) {
      EXPECT_FALSE(result.hasException());
      return Try<bool>(true);
    })
  );
  collect(saves).get();
  auto request1 = folly::make_unique<AddDocumentsToCentroidRequest>();
  request1->centroidId = "centroid-id";
  request1->documentIds = vector<string> {"doc-id"};
  auto response1 = ctx.server->addDocumentsToCentroid(
    std::move(request1)
  ).get();
  EXPECT_FALSE(response1.hasException());
  auto request2 = folly::make_unique<RemoveDocumentsFromCentroidRequest>();
  request2->centroidId = "centroid-id";
  request2->documentIds = vector<string> {"doc-id"};
  auto response2 = ctx.server->removeDocumentsFromCentroid(
    std::move(request2)
  ).get();
  EXPECT_FALSE(response2.hasException());
}

TEST(RelevanceServer, TestRemoveDocumentsFromCentroidDocumentNotInCentroid) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  vector<Future<Try<bool>>> saves;
  saves.push_back(
    ctx.persistence->saveCentroid("centroid-id", centroid)
  );
  saves.push_back(
    ctx.server->createDocumentWithID(
      folly::make_unique<string>("doc-id"),
      folly::make_unique<string>("some text about dogs"),
      Language::EN
    ).then([](Try<unique_ptr<string>> result) {
      EXPECT_FALSE(result.hasException());
      return Try<bool>(true);
    })
  );
  auto request = folly::make_unique<RemoveDocumentsFromCentroidRequest>();
  request->centroidId = "centroid-id";
  request->documentIds = vector<string> {"doc-id"};
  collect(saves).get();
  auto response = ctx.server->removeDocumentsFromCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<EDocumentNotInCentroid>());
}

TEST(RelevanceServer, TestRemoveDocumentFromCentroidMissingDocument) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  auto request = folly::make_unique<RemoveDocumentsFromCentroidRequest>();
  request->centroidId = "centroid-id";
  request->documentIds = vector<string> {"missing-doc-id"};
  auto response = ctx.server->removeDocumentsFromCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<EDocumentDoesNotExist>());
}

TEST(RelevanceServer, TestRemoveDocumentFromCentroidMissingCentroid) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request = folly::make_unique<RemoveDocumentsFromCentroidRequest>();
  request->centroidId = "missing-centroid-id";
  request->documentIds = vector<string> {"doc-id"};
  auto response = ctx.server->removeDocumentsFromCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<ECentroidDoesNotExist>());
}

TEST(RelevanceServer, TestRemoveDocumentFromCentroidMissingBoth) {
  RelevanceServerTestCtx ctx;
  auto centroid = std::make_shared<Centroid>(
    "centroid-id", unordered_map<string, double> {{"cats", 0.5}, {"dogs", 0.5}}, 15.3
  );
  ctx.persistence->saveCentroid("centroid-id", centroid).get();
  ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text about dogs"),
    Language::EN
  ).get();
  auto request = folly::make_unique<RemoveDocumentsFromCentroidRequest>();
  request->centroidId = "missing-centroid-id";
  request->documentIds = vector<string> {"missing-doc-id"};
  auto response = ctx.server->removeDocumentsFromCentroid(
    std::move(request)
  ).get();
  EXPECT_TRUE(response.hasException<ECentroidDoesNotExist>());
}