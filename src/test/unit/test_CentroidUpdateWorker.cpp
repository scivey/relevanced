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

#include "persistence/exceptions.h"
#include "TestHelpers.h"
#include "centroid_update_worker/CentroidUpdater.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "models/ProcessedDocument.h"

using namespace std;
using namespace wangle;
using namespace folly;
using namespace relevanced;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::document_processing_worker;
using namespace relevanced::models;


using persistence::exceptions::CentroidDoesNotExist;
using ::testing::Return;
using ::testing::_;

class MockCentroidUpdater : public CentroidUpdaterIf {
 public:
  MOCK_METHOD0(run, Try<bool>());
};

class MockCentroidUpdaterFactory : public CentroidUpdaterFactoryIf {
 public:
  MOCK_METHOD1(makeForCentroidId, shared_ptr<CentroidUpdaterIf>(const string&));
};

TEST(CentroidUpdateWorker, SimpleSuccess) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockCentroidUpdaterFactory updaterFactory;
  MockCentroidUpdater updater;

  shared_ptr<CentroidUpdaterIf> updaterPtr(&updater,
                                           NonDeleter<CentroidUpdaterIf>());

  shared_ptr<CentroidUpdaterFactoryIf> factoryPtr(
      &updaterFactory, NonDeleter<CentroidUpdaterFactoryIf>());

  EXPECT_CALL(updater, run()).WillOnce(Return(Try<bool>(true)));

  EXPECT_CALL(updaterFactory, makeForCentroidId("centroid-id"))
      .WillOnce(Return(updaterPtr));

  CentroidUpdateWorker worker(factoryPtr, threadPool);

  auto result = worker.update("centroid-id").get();
  EXPECT_FALSE(result.hasException());
  EXPECT_TRUE(result.value());
}

TEST(CentroidUpdateWorker, SimpleFailure) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockCentroidUpdaterFactory updaterFactory;
  MockCentroidUpdater updater;

  shared_ptr<CentroidUpdaterIf> updaterPtr(&updater,
                                           NonDeleter<CentroidUpdaterIf>());

  shared_ptr<CentroidUpdaterFactoryIf> factoryPtr(
      &updaterFactory, NonDeleter<CentroidUpdaterFactoryIf>());

  EXPECT_CALL(updater, run())
      .WillOnce(
          Return(Try<bool>(make_exception_wrapper<CentroidDoesNotExist>())));

  EXPECT_CALL(updaterFactory, makeForCentroidId("centroid-id"))
      .WillOnce(Return(updaterPtr));

  CentroidUpdateWorker worker(factoryPtr, threadPool);

  chrono::milliseconds updateDelay(0);
  auto result = worker.update("centroid-id", updateDelay).get();
  EXPECT_TRUE(result.hasException());
}
