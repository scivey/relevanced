#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <chrono>
#include <memory>

#include <glog/logging.h>
#include <folly/ExceptionWrapper.h>
#include <folly/futures/Try.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>


#include <folly/Optional.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "TestHelpers.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "persistence/InMemoryRockHandle.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/ThreadSafeUtf8Stemmer.h"
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "models/Document.h"

#include "MockSyncPersistence.h"
#include "util/util.h"
#include "text_util/ScoredWord.h"
#include "util/Clock.h"
#include "util/Sha1Hasher.h"


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
using namespace relevanced::tokenizer;
using thrift_protocol::ECentroidDoesNotExist;


using ::testing::Return;
using ::testing::_;


struct SimilarityWorkerTestCtx {
  shared_ptr<PersistenceIf> persistence;
  shared_ptr<CentroidMetadataDbIf> metadb;
  shared_ptr<Sha1HasherIf> hasher;
  shared_ptr<ClockIf> sysClock;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool1;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool2;
  shared_ptr<SimilarityScoreWorker> worker;

  SimilarityWorkerTestCtx() {
    threadPool1.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    threadPool2.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    UniquePointer<RockHandleIf> rockHandle(new InMemoryRockHandle("foo"));
    UniquePointer<SyncPersistenceIf> syncPersistence(
      new SyncPersistence(std::move(rockHandle))
    );
    persistence.reset(new Persistence(std::move(syncPersistence), threadPool1));
    hasher.reset(new Sha1Hasher);
    metadb.reset(new CentroidMetadataDb(persistence));
    sysClock.reset(new Clock);
    worker.reset(new SimilarityScoreWorker(
      persistence, metadb, threadPool2
    ));
  }
};

TEST(SimilarityScoreWorker, SimpleReload) {
  SimilarityWorkerTestCtx ctx;
  auto sharedCentroid = std::make_shared<Centroid>(
    "centroid-id",
    map<string, double>{ {"cat", 5.3}, {"dog", 4.8 } },
    17.5
  );
  auto nothing = ctx.worker->debugGetCentroid("centroid-id");
  EXPECT_FALSE(nothing.hasValue());
  ctx.persistence->saveCentroid("centroid-id", sharedCentroid).get();
  ctx.worker->reloadCentroid("centroid-id").get();
  auto reloaded = ctx.worker->debugGetCentroid("centroid-id");
  EXPECT_TRUE(reloaded.hasValue());
  EXPECT_EQ("centroid-id", reloaded.value()->id);
}

TEST(SimilarityScoreWorker, ScoreExisting) {
  SimilarityWorkerTestCtx ctx;
  auto sharedCentroid = std::make_shared<Centroid>(
    "centroid-id",
    map<string, double>{ {"cat", 5.3}, {"fish", 4.8 } },
    17.5
  );
  ctx.persistence->saveCentroid("centroid-id", sharedCentroid).get();
  ctx.worker->reloadCentroid("centroid-id").get();
  vector<ScoredWord> docWords {
    ScoredWord("fish", 4, 0.5),
    ScoredWord("cat", 3, 0.5)
  };
  auto doc = std::make_shared<ProcessedDocument>(
    "doc-id", docWords, 1.0
  );
  auto result = ctx.worker->getDocumentSimilarity("centroid-id", doc).get();
  EXPECT_TRUE(result.hasValue());
  auto val = result.value();
  EXPECT_TRUE(val > 0);
  EXPECT_TRUE(val < 1);
}

TEST(SimilarityScoreWorker, ScoreMissingCentroid) {
  SimilarityWorkerTestCtx ctx;
  vector<ScoredWord> docWords {
    ScoredWord("fish", 4, 0.5),
    ScoredWord("cat", 3, 0.5)
  };
  auto doc = std::make_shared<ProcessedDocument>(
    "doc-id", docWords, 1.0
  );
  auto result = ctx.worker->getDocumentSimilarity("centroid-id", doc).get();
  EXPECT_TRUE(result.hasException<ECentroidDoesNotExist>());
}

