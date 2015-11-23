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

TEST(RelevanceServer, TestCreateDocumentWithID) {
  RelevanceServerTestCtx ctx;
  auto text = folly::make_unique<string>("some text about cats and dogs and fish and so forth");
  auto id = folly::make_unique<string>("doc-id");
  auto response = ctx.server->createDocumentWithID(
    std::move(id), std::move(text), Language::EN
  ).get();
  EXPECT_TRUE(response.hasValue());
  EXPECT_EQ("doc-id", *response.value());
  auto persisted = ctx.persistence->loadDocument("doc-id").get();
  EXPECT_TRUE(persisted.hasValue());
  EXPECT_EQ("doc-id", persisted.value()->id);
}

TEST(RelevanceServer, TestCreateDocumentWithIDAlreadyExists) {
  RelevanceServerTestCtx ctx;
  auto text = folly::make_unique<string>("some text about cats and dogs and fish and so forth");
  auto id = folly::make_unique<string>("doc-id");
  auto response1 = ctx.server->createDocumentWithID(
    std::move(id), std::move(text), Language::EN
  ).get();
  EXPECT_FALSE(response1.hasException());
  auto response2 = ctx.server->createDocumentWithID(
    folly::make_unique<string>("doc-id"),
    folly::make_unique<string>("some text"),
    Language::EN
  ).get();
  EXPECT_TRUE(response2.hasException<EDocumentAlreadyExists>());
}

TEST(RelevanceServer, TestListAllDocuments) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<unique_ptr<string>>>> creations;
  set<string> expectedIds;
  for (size_t i = 0; i < 10; i++) {
    auto id = sformat("some-doc-{}", i);
    expectedIds.insert(id);
    creations.push_back(ctx.server->createDocumentWithID(
      folly::make_unique<string>(id),
      folly::make_unique<string>("this is some text about things"),
      Language::EN
    ));
  }
  set<string> createdIds;
  auto created = collect(creations).get();
  for (auto &id: created) {
    EXPECT_TRUE(id.hasValue());
    createdIds.insert(*id.value());
  }
  EXPECT_EQ(expectedIds, createdIds);

  auto documents = ctx.server->listAllDocuments().get();
  set<string> returnedIds;
  for (auto &id: *documents) {
    returnedIds.insert(id);
  }
  EXPECT_EQ(expectedIds, returnedIds);
}

TEST(RelevanceServer, TestListDocumentRange) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<unique_ptr<string>>>> creations;
  for (size_t i = 0; i < 10; i++) {
    auto id = sformat("some-doc-{}", i);
    creations.push_back(ctx.server->createDocumentWithID(
      folly::make_unique<string>(id),
      folly::make_unique<string>("this is some text about things"),
      Language::EN
    ));
  }
  collect(creations).get();
  auto documents = ctx.server->listDocumentRange(3, 4).get();
  set<string> returnedIds;
  for (auto &id: *documents) {
    returnedIds.insert(id);
  }
  set<string> expectedIds {
    "some-doc-3", "some-doc-4", "some-doc-5", "some-doc-6"
  };
  EXPECT_EQ(expectedIds, returnedIds);
}

TEST(RelevanceServer, TestListDocumentRangeFromID) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<unique_ptr<string>>>> creations;
  for (size_t i = 0; i < 10; i++) {
    auto id = sformat("some-doc-{}", i);
    creations.push_back(ctx.server->createDocumentWithID(
      folly::make_unique<string>(id),
      folly::make_unique<string>("this is some text about things"),
      Language::EN
    ));
  }
  collect(creations).get();
  auto documents = ctx.server->listDocumentRangeFromID(
    folly::make_unique<string>("some-doc-4"), 3
  ).get();
  set<string> returnedIds;
  for (auto &id: *documents) {
    returnedIds.insert(id);
  }
  set<string> expectedIds {
    "some-doc-4", "some-doc-5", "some-doc-6"
  };
  EXPECT_EQ(expectedIds, returnedIds);
}

TEST(RelevanceServer, TestDeleteDocument) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<unique_ptr<string>>>> creations;
  for (size_t i = 0; i < 6; i++) {
    auto id = sformat("some-doc-{}", i);
    creations.push_back(ctx.server->createDocumentWithID(
      folly::make_unique<string>(id),
      folly::make_unique<string>("this is some text about things"),
      Language::EN
    ));
  }
  collect(creations).get();
  bool ignoreMissing = false;
  ctx.server->deleteDocument(
    folly::make_unique<string>("some-doc-3"), ignoreMissing
  ).get();
  ctx.server->deleteDocument(
    folly::make_unique<string>("some-doc-4"), ignoreMissing
  ).get();
  set<string> expectedIds {
    "some-doc-0", "some-doc-1", "some-doc-2", "some-doc-5"
  };
  auto documents = ctx.server->listAllDocuments().get();
  set<string> returnedIds;
  for (auto &id: *documents) {
    returnedIds.insert(id);
  }
  EXPECT_EQ(expectedIds, returnedIds);
}

TEST(RelevanceServer, TestDeleteDocumentMissing) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<unique_ptr<string>>>> creations;
  for (size_t i = 0; i < 6; i++) {
    auto id = sformat("some-doc-{}", i);
    creations.push_back(ctx.server->createDocumentWithID(
      folly::make_unique<string>(id),
      folly::make_unique<string>("this is some text about things"),
      Language::EN
    ));
  }
  collect(creations).get();
  bool ignoreMissing = false;
  auto result = ctx.server->deleteDocument(
    folly::make_unique<string>("some-doc-8"), ignoreMissing
  ).get();
  EXPECT_TRUE(result.hasException<EDocumentDoesNotExist>());
}

TEST(RelevanceServer, TestListUnusedDocuments) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<unique_ptr<string>>>> documentCreations;
  for (size_t i = 0; i < 6; i++) {
    auto id = sformat("some-doc-{}", i);
    documentCreations.push_back(ctx.server->createDocumentWithID(
      folly::make_unique<string>(id),
      folly::make_unique<string>("this is some text about things"),
      Language::EN
    ));
  }
  collect(documentCreations).get();
  bool ignoreExisting = false;
  ctx.server->createCentroid(
    folly::make_unique<string>("c1"), ignoreExisting
  ).get();
  vector<Future<Try<bool>>> additions;
  vector<string> toAdd {"some-doc-1", "some-doc-3", "some-doc-4"};
  for (string id: toAdd) {
    string cId = "c1";
    additions.push_back(ctx.server->addOneDocumentToCentroid(
      cId, id, false, false
    ));
  }
  collect(additions).get();
  auto unused = ctx.server->listUnusedDocuments(10).get();
  vector<string> result = *unused;
  vector<string> expected {
    "some-doc-0", "some-doc-2", "some-doc-5"
  };
  EXPECT_EQ(expected, result);
}
