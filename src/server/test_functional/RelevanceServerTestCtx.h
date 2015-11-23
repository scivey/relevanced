#pragma once
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


struct RelevanceServerTestCtx {
  shared_ptr<PersistenceIf> persistence;
  shared_ptr<CentroidMetadataDbIf> metadb;
  shared_ptr<Sha1HasherIf> hasher;
  shared_ptr<ClockIf> sysClock;
  shared_ptr<StemmerManagerIf> stemmerManager;
  shared_ptr<StopwordFilterIf> stopwordFilter;
  shared_ptr<DocumentProcessorIf> processor;
  shared_ptr<CentroidUpdaterFactoryIf> updaterFactory;
  shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> persistenceThreads;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> processingThreads;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> scoringThreads;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> updatingThreads;
  shared_ptr<SimilarityScoreWorker> scoreWorker;
  shared_ptr<DocumentProcessingWorker> processingWorker;
  shared_ptr<CentroidUpdateWorker> updateWorker;
  shared_ptr<RelevanceServer> server;

  RelevanceServerTestCtx(bool initialize = true) {
    persistenceThreads.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    processingThreads.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    scoringThreads.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    updatingThreads.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    UniquePointer<RockHandleIf> rockHandle(new InMemoryRockHandle("foo"));
    sysClock.reset(new Clock);
    UniquePointer<SyncPersistenceIf> syncPersistence(
      new SyncPersistence(sysClock, std::move(rockHandle))
    );
    persistence.reset(new Persistence(std::move(syncPersistence), persistenceThreads));
    hasher.reset(new Sha1Hasher);
    metadb.reset(new CentroidMetadataDb(persistence));
    stemmerManager.reset(new ThreadSafeStemmerManager);
    stopwordFilter.reset(new StopwordFilter);
    processor.reset(
      new DocumentProcessor(stemmerManager, stopwordFilter, sysClock)
    );
    accumulatorFactory.reset(new DocumentAccumulatorFactory);
    updaterFactory.reset(new CentroidUpdaterFactory(
      persistence, metadb, accumulatorFactory, sysClock
    ));
    processingWorker.reset(new DocumentProcessingWorker(
      processor, hasher, processingThreads
    ));
    scoreWorker.reset(new SimilarityScoreWorker(
      persistence, metadb, scoringThreads
    ));
    updateWorker.reset(new CentroidUpdateWorker(
      updaterFactory, updatingThreads
    ));
    server.reset(new RelevanceServer(
      persistence, metadb, sysClock, scoreWorker, processingWorker, updateWorker
    ));
    if (initialize) {
      server->initialize();
      updateWorker->debug_getUpdateQueue()->debug_setVeryShortTimeouts();
    }
  }
  ~RelevanceServerTestCtx() {
    updateWorker->join();
  }
};
