#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <chrono>
#include <memory>

#include <glog/logging.h>
#include <folly/ExceptionWrapper.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "TestHelpers.h"
#include "persistence/exceptions.h"
#include "persistence/Persistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "MockSyncPersistence.h"
#include "util/util.h"

using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;
using namespace relevanced::util;
using ::testing::Return;
using ::testing::_;

shared_ptr<CentroidMetadataDb> makeMetadataDb(
    MockSyncPersistence &syncPersistence) {
  UniquePointer<SyncPersistenceIf> syncPersistencePtr(
      &syncPersistence, NonDeleter<SyncPersistenceIf>());
  auto threadPool = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  shared_ptr<PersistenceIf> persistencePtr(
      new Persistence(std::move(syncPersistencePtr), threadPool));
  return std::make_shared<CentroidMetadataDb>(persistencePtr);
}

TEST(TestCentroidMetadataDb, TestGetCreatedTimestampHappy) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  string metaVal{"1234"};
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "created"))
      .WillOnce(Return(metaVal));
  auto result = metaDb->getCreatedTimestamp("centroid-id").get();
  EXPECT_TRUE(result.hasValue());
  EXPECT_EQ(1234, result.value());
}

TEST(TestCentroidMetadataDb, TestGetCreatedTimestampMissing) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Optional<string> metaVal;
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "created"))
      .WillOnce(Return(metaVal));
  auto result = metaDb->getCreatedTimestamp("centroid-id").get();
  EXPECT_FALSE(result.hasValue());
}

TEST(TestCentroidMetadataDb, TestGetLastCalculatedTimestamp) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  string metaVal{"1234"};
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(metaVal));
  auto result = metaDb->getLastCalculatedTimestamp("centroid-id").get();
  EXPECT_TRUE(result.hasValue());
  EXPECT_EQ(1234, result.value());
}

TEST(TestCentroidMetadataDb, TestGetLastCalculatedTimestampMissing) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Optional<string> metaVal;
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(metaVal));
  auto result = metaDb->getLastCalculatedTimestamp("centroid-id").get();
  EXPECT_FALSE(result.hasValue());
}

TEST(TestCentroidMetadataDb, TestGetLastDocumentChangeTimestamp) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  string metaVal{"1234"};
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(metaVal));
  auto result = metaDb->getLastDocumentChangeTimestamp("centroid-id").get();
  EXPECT_TRUE(result.hasValue());
  EXPECT_EQ(1234, result.value());
}

TEST(TestCentroidMetadataDb, TestGetLastDocumentChangeTimestampMissing) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Optional<string> metaVal;
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(metaVal));
  auto result = metaDb->getLastDocumentChangeTimestamp("centroid-id").get();
  EXPECT_FALSE(result.hasValue());
}

TEST(TestCentroidMetadataDb, TestIsCentroidUpToDateHaveBothUpToDate) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  string lastDocumentChange{"1000"};
  string lastCalculated{"1050"};
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(lastDocumentChange));
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(lastCalculated));
  auto result = metaDb->isCentroidUpToDate("centroid-id").get();
  EXPECT_FALSE(result.hasException());
  EXPECT_TRUE(result.value());
}

TEST(TestCentroidMetadataDb, TestIsCentroidUpToDateHaveBothOutOfDate) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  string lastDocumentChange{"1000"};
  string lastCalculated{"900"};
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(lastDocumentChange));
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(lastCalculated));
  auto result = metaDb->isCentroidUpToDate("centroid-id").get();
  EXPECT_FALSE(result.hasException());
  EXPECT_FALSE(result.value());
}

TEST(TestCentroidMetadataDb, TestIsCentroidUpToDateMissingLastChange) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Optional<string> lastDocumentChange;
  string lastCalculated{"900"};
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(lastDocumentChange));
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(lastCalculated));
  auto result = metaDb->isCentroidUpToDate("centroid-id").get();
  EXPECT_FALSE(result.hasException());
  EXPECT_TRUE(result.value());
}

TEST(TestCentroidMetadataDb, TestIsCentroidUpToDateMissingLastCalculated) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  string lastDocumentChange{"1000"};
  Optional<string> lastCalculated;
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(lastDocumentChange));
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(lastCalculated));
  auto result = metaDb->isCentroidUpToDate("centroid-id").get();
  EXPECT_FALSE(result.hasException());
  EXPECT_FALSE(result.value());
}

TEST(TestCentroidMetadataDb, TestIsCentroidUpToDateMissingBoth) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Optional<string> lastDocumentChange;
  Optional<string> lastCalculated;
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastDocumentChange"))
      .WillOnce(Return(lastDocumentChange));
  EXPECT_CALL(syncDb, getCentroidMetadata("centroid-id", "lastCalculated"))
      .WillOnce(Return(lastCalculated));
  auto result = metaDb->isCentroidUpToDate("centroid-id").get();
  EXPECT_FALSE(result.hasException());
  EXPECT_TRUE(result.value());
}

TEST(TestCentroidMetadataDb, TestSetCreatedTimestamp) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Try<bool> respondWith(true);
  EXPECT_CALL(syncDb, setCentroidMetadata("centroid-id", "created", "1234"))
      .WillOnce(Return(respondWith));
  auto result = metaDb->setCreatedTimestamp("centroid-id", 1234).get();
  EXPECT_FALSE(result.hasException());
}

TEST(TestCentroidMetadataDb, TestSetLastCalculatedTimestamp) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Try<bool> respondWith(true);
  EXPECT_CALL(syncDb,
              setCentroidMetadata("centroid-id", "lastCalculated", "1234"))
      .WillOnce(Return(respondWith));
  auto result = metaDb->setLastCalculatedTimestamp("centroid-id", 1234).get();
  EXPECT_FALSE(result.hasException());
}

TEST(TestCentroidMetadataDb, TestSetLastDocumentChangeTimestamp) {
  MockSyncPersistence syncDb;
  auto metaDb = makeMetadataDb(syncDb);
  Try<bool> respondWith(true);
  EXPECT_CALL(syncDb,
              setCentroidMetadata("centroid-id", "lastDocumentChange", "1234"))
      .WillOnce(Return(respondWith));
  auto result =
      metaDb->setLastDocumentChangeTimestamp("centroid-id", 1234).get();
  EXPECT_FALSE(result.hasException());
}
