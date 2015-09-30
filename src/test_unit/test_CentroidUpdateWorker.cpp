#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "TestHelpers.h"
#include "CentroidUpdater.h"
#include "CentroidUpdaterFactory.h"
#include "CentroidUpdateWorker.h"
#include "DocumentProcessingWorker.h"
#include "ProcessedDocument.h"

using namespace std;
using namespace wangle;
using stopwords::StopwordFilterIf;
using stemmer::StemmerIf;
using tokenizer::TokenizerIf;
using ::testing::Return;
using ::testing::_;

class MockCentroidUpdater: public CentroidUpdaterIf {
public:
  MOCK_METHOD0(run, bool());
};

class MockCentroidUpdaterFactory: public CentroidUpdaterFactoryIf {
public:
  MOCK_METHOD1(makeForCentroidId, shared_ptr<CentroidUpdaterIf>(const string&));
};

TEST(CentroidUpdateWorker, SimpleSuccess) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockCentroidUpdaterFactory updaterFactory;
  MockCentroidUpdater updater;

  shared_ptr<CentroidUpdaterIf> updaterPtr(
    &updater, NonDeleter<CentroidUpdaterIf>()
  );

  shared_ptr<CentroidUpdaterFactoryIf> factoryPtr(
    &updaterFactory, NonDeleter<CentroidUpdaterFactoryIf>()
  );

  EXPECT_CALL(updater, run())
    .WillOnce(Return(true));

  EXPECT_CALL(updaterFactory, makeForCentroidId("centroid-id"))
    .WillOnce(Return(updaterPtr));

  CentroidUpdateWorker worker(factoryPtr, threadPool);

  auto result = worker.update("centroid-id").get();
  EXPECT_TRUE(result);
}

TEST(CentroidUpdateWorker, SimpleFailure) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockCentroidUpdaterFactory updaterFactory;
  MockCentroidUpdater updater;

  shared_ptr<CentroidUpdaterIf> updaterPtr(
    &updater, NonDeleter<CentroidUpdaterIf>()
  );

  shared_ptr<CentroidUpdaterFactoryIf> factoryPtr(
    &updaterFactory, NonDeleter<CentroidUpdaterFactoryIf>()
  );

  EXPECT_CALL(updater, run())
    .WillOnce(Return(false));

  EXPECT_CALL(updaterFactory, makeForCentroidId("centroid-id"))
    .WillOnce(Return(updaterPtr));

  CentroidUpdateWorker worker(factoryPtr, threadPool);

  chrono::milliseconds updateDelay(0);
  auto result = worker.update("centroid-id", updateDelay).get();
  EXPECT_FALSE(result);
}
