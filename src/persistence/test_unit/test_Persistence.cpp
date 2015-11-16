#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <chrono>
#include <memory>

#include <folly/ExceptionWrapper.h>
#include <folly/futures/Try.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "testing/TestHelpers.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "testing/MockSyncPersistence.h"
#include "util/util.h"
using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;
using namespace relevanced::util;
using ::testing::Return;
using ::testing::_;

shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> getThreadPool() {
  static auto threads =
      std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  return threads;
}

// TEST(TestPersistence, TestDoesDocumentExist1) {
//   MockSyncPersistence syncPersistence;

//   UniquePointer<SyncPersistenceIf> syncPersistencePtr(
//       (SyncPersistenceIf*) &syncPersistence, NonDeleter<SyncPersistenceIf>());

//   Persistence persistence(std::move(syncPersistencePtr), getThreadPool());
//   string id = "doc-id";
//   EXPECT_CALL(syncPersistence, doesDocumentExist(id)).WillOnce(Return(true));
//   auto result = persistence.doesDocumentExist(id).get();
//   EXPECT_TRUE(result);
// }

// TEST(TestPersistence, TestDoesDocumentExist2) {
//   MockSyncPersistence syncPersistence;

//   UniquePointer<SyncPersistenceIf> syncPersistencePtr(
//       (SyncPersistenceIf*) &syncPersistence, NonDeleter<SyncPersistenceIf>());

//   Persistence persistence(std::move(syncPersistencePtr), getThreadPool());
//   string id = "doc-id";
//   EXPECT_CALL(syncPersistence, doesDocumentExist(id)).WillOnce(Return(false));
//   auto result = persistence.doesDocumentExist(id).get();
//   EXPECT_FALSE(result);
// }

// TEST(TestPersistence, TestDoesCentroidExist1) {
//   MockSyncPersistence syncPersistence;

//   UniquePointer<SyncPersistenceIf> syncPersistencePtr(
//       (SyncPersistenceIf*) &syncPersistence, NonDeleter<SyncPersistenceIf>());

//   Persistence persistence(std::move(syncPersistencePtr), getThreadPool());
//   string id = "centroid-id";
//   EXPECT_CALL(syncPersistence, doesCentroidExist(id)).WillOnce(Return(true));
//   auto result = persistence.doesCentroidExist(id).get();
//   EXPECT_TRUE(result);
// }

// TEST(TestPersistence, TestDoesCentroidExist2) {
//   MockSyncPersistence syncPersistence;

//   UniquePointer<SyncPersistenceIf> syncPersistencePtr(
//       (SyncPersistenceIf*) &syncPersistence, NonDeleter<SyncPersistenceIf>());

//   Persistence persistence(std::move(syncPersistencePtr), getThreadPool());
//   string id = "centroid-id";
//   EXPECT_CALL(syncPersistence, doesCentroidExist(id)).WillOnce(Return(false));
//   auto result = persistence.doesCentroidExist(id).get();
//   EXPECT_FALSE(result);
// }
