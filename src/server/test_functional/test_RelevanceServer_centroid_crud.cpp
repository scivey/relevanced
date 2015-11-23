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

TEST(RelevanceServer, TestCreateCentroid) {
  RelevanceServerTestCtx ctx;
  auto id = folly::make_unique<string>("some-centroid");
  bool ignoreExisting = false;
  auto response = ctx.server->createCentroid(std::move(id), ignoreExisting).get();
  EXPECT_TRUE(response.hasValue());
  EXPECT_TRUE(response.value());
  auto persisted = ctx.persistence->loadCentroid("some-centroid").get();
  EXPECT_TRUE(persisted.hasValue());
  EXPECT_EQ("some-centroid", persisted.value()->id);
}

TEST(RelevanceServer, TestCreateCentroidAlreadyExists) {
  RelevanceServerTestCtx ctx;
  auto id = folly::make_unique<string>("some-centroid");
  bool ignoreExisting = false;
  auto response1 = ctx.server->createCentroid(
    folly::make_unique<string>("some-centroid"), ignoreExisting
  ).get();
  EXPECT_FALSE(response1.hasException());
  auto response2 = ctx.server->createCentroid(
    folly::make_unique<string>("some-centroid"), ignoreExisting
  ).get();
  EXPECT_TRUE(response2.hasException<ECentroidAlreadyExists>());
}

TEST(RelevanceServer, TestCreateCentroidAlreadyExistsIgnoreExisting) {
  RelevanceServerTestCtx ctx;
  auto id = folly::make_unique<string>("some-centroid");
  bool ignoreExisting = true;
  auto response1 = ctx.server->createCentroid(
    folly::make_unique<string>("some-centroid"), ignoreExisting
  ).get();
  EXPECT_FALSE(response1.hasException());
  auto response2 = ctx.server->createCentroid(
    folly::make_unique<string>("some-centroid"), ignoreExisting
  ).get();
  EXPECT_FALSE(response2.hasException<ECentroidAlreadyExists>());
}

TEST(RelevanceServer, TestMultiCreateCentroid) {
  RelevanceServerTestCtx ctx;
  bool ignoreExisting = false;
  ctx.server->createCentroid(
    folly::make_unique<string>("old-centroid"), ignoreExisting
  ).get();
  auto centroidList1 = ctx.server->listAllCentroids().get();
  vector<string> expected1 {"old-centroid"};
  EXPECT_EQ(
    expected1,
    *centroidList1
  );
  ctx.server->multiCreateCentroids(
    folly::make_unique<vector<string>>(
      vector<string> {"c1", "c2", "c3"}
    ),
    ignoreExisting
  ).get();
  auto centroidList2 = ctx.server->listAllCentroids().get();
  vector<string> expected2 {"c1", "c2", "c3", "old-centroid"};
  EXPECT_EQ(
    expected2,
    *centroidList2
  );
}

TEST(RelevanceServer, TestMultiCreateCentroidOneExists) {
  RelevanceServerTestCtx ctx;
  bool ignoreExisting = false;
  ctx.server->createCentroid(
    folly::make_unique<string>("old-centroid"), ignoreExisting
  ).get();
  auto centroidList1 = ctx.server->listAllCentroids().get();
  vector<string> expected1 {"old-centroid"};
  EXPECT_EQ(
    expected1,
    *centroidList1
  );
  auto result = ctx.server->multiCreateCentroids(
    folly::make_unique<vector<string>>(
      vector<string> {"c1", "old-centroid", "c3"}
    ),
    ignoreExisting
  ).get();
  auto errResult = result.at(1);
  EXPECT_TRUE(errResult.hasException());
  auto centroidList2 = ctx.server->listAllCentroids().get();
  vector<string> expected2 {"c1", "c3", "old-centroid"};
  EXPECT_EQ(
    expected2,
    *centroidList2
  );
}

TEST(RelevanceServer, TestListAllCentroids) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<bool>>> creations;
  set<string> expectedIds;
  for (size_t i = 0; i < 10; i++) {
    auto id = sformat("some-centroid-{}", i);
    expectedIds.insert(id);
    bool ignoreExisting = false;
    creations.push_back(ctx.server->createCentroid(
      folly::make_unique<string>(id), ignoreExisting
    ));
  }
  collect(creations).get();
  auto centroids = ctx.server->listAllCentroids().get();
  set<string> returnedIds;
  for (auto &id: *centroids) {
    returnedIds.insert(id);
  }
  EXPECT_EQ(expectedIds, returnedIds);
}

TEST(RelevanceServer, TestListCentroidRange) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<bool>>> creations;
  for (size_t i = 0; i < 10; i++) {
    auto id = sformat("some-centroid-{}", i);
    bool ignoreExisting = false;
    creations.push_back(ctx.server->createCentroid(
      folly::make_unique<string>(id), ignoreExisting
    ));
  }
  collect(creations).get();
  auto centroids = ctx.server->listCentroidRange(3, 4).get();
  set<string> returnedIds;
  for (auto &id: *centroids) {
    returnedIds.insert(id);
  }
  set<string> expectedIds {
    "some-centroid-3", "some-centroid-4", "some-centroid-5", "some-centroid-6"
  };
  EXPECT_EQ(expectedIds, returnedIds);
}

TEST(RelevanceServer, TestListCentroidRangeFromID) {
  RelevanceServerTestCtx ctx;
  vector<Future<Try<bool>>> creations;
  for (size_t i = 0; i < 10; i++) {
    auto id = sformat("some-centroid-{}", i);
    bool ignoreExisting = false;
    creations.push_back(ctx.server->createCentroid(
      folly::make_unique<string>(id), ignoreExisting
    ));
  }
  collect(creations).get();
  auto centroids = ctx.server->listCentroidRangeFromID(
    folly::make_unique<string>("some-centroid-5"), 3
  ).get();
  set<string> returnedIds;
  for (auto &id: *centroids) {
    returnedIds.insert(id);
  }
  set<string> expectedIds {
    "some-centroid-5", "some-centroid-6", "some-centroid-7"
  };
  EXPECT_EQ(expectedIds, returnedIds);
}
