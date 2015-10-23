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
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "persistence/InMemoryRockHandle.h"
#include "centroid_update_worker/DocumentAccumulatorFactory.h"
#include "centroid_update_worker/DocumentAccumulator.h"
#include "centroid_update_worker/CentroidUpdater.h"
#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"

#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "MockSyncPersistence.h"
#include "util/util.h"
#include "util/Clock.h"
#include "text_util/ScoredWord.h"


using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;
using namespace relevanced::util;
using namespace relevanced::text_util;
using namespace relevanced::centroid_update_worker;

using ::testing::Return;
using ::testing::_;


struct UpdateWorkerTestCtx {
  shared_ptr<PersistenceIf> persistence;
  shared_ptr<CentroidMetadataDbIf> metadb;
  shared_ptr<CentroidUpdaterFactoryIf> updaterFactory;
  shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory;
  shared_ptr<ClockIf> sysClock;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool1;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool2;
  shared_ptr<CentroidUpdateWorker> updateWorker;
  UpdateWorkerTestCtx() {
    threadPool1.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    threadPool2.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    UniquePointer<RockHandleIf> rockHandle(new InMemoryRockHandle("foo"));
    UniquePointer<SyncPersistenceIf> syncPersistence(
      new SyncPersistence(std::move(rockHandle))
    );
    shared_ptr<PersistenceIf> result(
      new Persistence(std::move(syncPersistence), threadPool1)
    );
    persistence = result;
    metadb.reset(new CentroidMetadataDb(persistence));
    accumulatorFactory.reset(new DocumentAccumulatorFactory);
    sysClock.reset(new Clock);
    updaterFactory.reset(new CentroidUpdaterFactory(
      persistence, metadb, accumulatorFactory, sysClock
    ));
    updateWorker.reset(new CentroidUpdateWorker(updaterFactory, threadPool2));
  }
};

TEST(CentroidUpdateWorker, Simple) {
  UpdateWorkerTestCtx ctx;
  vector<ScoredWord> doc1Words {
    ScoredWord("cat", 3, 0.25),
    ScoredWord("fish", 4, 0.5),
    ScoredWord("mouse", 5, 0.25)
  };
  vector<ScoredWord> doc2Words {
    ScoredWord("cat", 3, 0.25),
    ScoredWord("food", 4, 0.5),
    ScoredWord("nothing", 7, 0.25)
  };
  auto doc1 = make_shared<ProcessedDocument>(
    "doc1", doc1Words, 1.0
  );
  auto doc2 = make_shared<ProcessedDocument>(
    "doc2", doc1Words, 1.0
  );
  ctx.persistence->saveDocument(doc1).get();
  ctx.persistence->saveDocument(doc2).get();
  ctx.persistence->createNewCentroid("some-centroid").get();
  ctx.persistence->addDocumentToCentroid("some-centroid", "doc1").get();
  ctx.persistence->addDocumentToCentroid("some-centroid", "doc2").get();
  ctx.updateWorker->update("some-centroid", chrono::milliseconds(1)).get();

  auto persisted = ctx.persistence->loadCentroidOption("some-centroid").get();
  EXPECT_TRUE(persisted.hasValue());
  auto result = persisted.value();
  EXPECT_EQ("some-centroid", result->id);
}


TEST(CentroidUpdateWorker, JoinUpdateSimple) {
  UpdateWorkerTestCtx ctx;
  vector<ScoredWord> doc1Words {
    ScoredWord("cat", 3, 0.25),
    ScoredWord("fish", 4, 0.5),
    ScoredWord("mouse", 5, 0.25)
  };
  vector<ScoredWord> doc2Words {
    ScoredWord("cat", 3, 0.25),
    ScoredWord("food", 4, 0.5),
    ScoredWord("nothing", 7, 0.25)
  };
  auto doc1 = make_shared<ProcessedDocument>(
    "doc1", doc1Words, 1.0
  );
  auto doc2 = make_shared<ProcessedDocument>(
    "doc2", doc1Words, 1.0
  );
  ctx.persistence->saveDocument(doc1).get();
  ctx.persistence->saveDocument(doc2).get();
  ctx.persistence->createNewCentroid("some-centroid").get();
  ctx.persistence->addDocumentToCentroid("some-centroid", "doc1").get();
  ctx.persistence->addDocumentToCentroid("some-centroid", "doc2").get();
  ctx.updateWorker->joinUpdate("some-centroid").get();

  auto persisted = ctx.persistence->loadCentroidOption("some-centroid").get();
  EXPECT_TRUE(persisted.hasValue());
  auto result = persisted.value();
  EXPECT_EQ("some-centroid", result->id);
}


TEST(CentroidUpdateWorker, JoinUpdateAlreadyInProgress) {
  UpdateWorkerTestCtx ctx;
  vector<ScoredWord> doc1Words {
    ScoredWord("cat", 3, 0.25),
    ScoredWord("fish", 4, 0.5),
    ScoredWord("mouse", 5, 0.25)
  };
  vector<ScoredWord> doc2Words {
    ScoredWord("cat", 3, 0.25),
    ScoredWord("food", 4, 0.5),
    ScoredWord("nothing", 7, 0.25)
  };
  auto doc1 = make_shared<ProcessedDocument>(
    "doc1", doc1Words, 1.0
  );
  auto doc2 = make_shared<ProcessedDocument>(
    "doc2", doc1Words, 1.0
  );
  ctx.persistence->saveDocument(doc1).get();
  ctx.persistence->saveDocument(doc2).get();
  ctx.persistence->createNewCentroid("some-centroid").get();
  ctx.persistence->addDocumentToCentroid("some-centroid", "doc1").get();
  ctx.persistence->addDocumentToCentroid("some-centroid", "doc2").get();
  auto updateFuture = ctx.updateWorker->update("some-centroid", chrono::milliseconds(1));
  ctx.updateWorker->joinUpdate("some-centroid").get();
  EXPECT_TRUE(updateFuture.hasValue());

  auto persisted = ctx.persistence->loadCentroidOption("some-centroid").get();
  EXPECT_TRUE(persisted.hasValue());
  auto result = persisted.value();
  EXPECT_EQ("some-centroid", result->id);
}