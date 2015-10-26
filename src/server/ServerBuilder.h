#pragma once
#include <cassert>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "centroid_update_worker/DocumentAccumulatorFactory.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/RockHandle.h"
#include "persistence/CentroidMetadataDb.h"
#include "server/RelevanceServer.h"
#include "server/ThriftRelevanceServer.h"
#include "util/util.h"
#include "util/Clock.h"
#include "util/Sha1Hasher.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "server/RelevanceServerOptions.h"


namespace relevanced {
namespace server {

using namespace std;
using util::UniquePointer;
using namespace std;
using namespace folly;
using namespace persistence;
using namespace similarity_score_worker;
using namespace centroid_update_worker;
using namespace document_processing_worker;
using relevanced::stemmer::StemmerIf;
using relevanced::stopwords::StopwordFilter;
using relevanced::stopwords::StopwordFilterIf;
using wangle::CPUThreadPoolExecutor;
using wangle::FutureExecutor;
using util::UniquePointer;

/**
 * This is an AbstractNonsenseArchitectureAstronautFactoryManager.
 *
 * There is some indirection in the rest of the code for the sake of
 * testability.  For instance, the `DocumentProcessor` and `CentroidUpdater` are
 * defined in terms of interfaces even though the "real" `RelevanceServer`
 * only needs one implementation of each.
 *
 * `ServerBuilder`'s job is to stitch those layers of indirection together into
 * a usable `RelevancedServer` or `ThriftRelevanceServer` instance.
 *
 * The main goal here is to contain the abstract nonsense to one place.
 */
class ServerBuilder {
  shared_ptr<PersistenceIf> persistence_;
  shared_ptr<CentroidMetadataDbIf> centroidMetadataDb_;
  shared_ptr<DocumentProcessingWorkerIf> processor_;
  shared_ptr<SimilarityScoreWorkerIf> similarityWorker_;
  shared_ptr<CentroidUpdateWorkerIf> centroidUpdater_;
  shared_ptr<RelevanceServerOptions> options_;
  shared_ptr<util::ClockIf> clock_;
  shared_ptr<util::Sha1HasherIf> hasher_;

 public:
  ServerBuilder(shared_ptr<RelevanceServerOptions> options)
      : options_(options) {}

  template <typename RockHandleT,
            typename SyncPersistenceT,
            typename PersistenceT,
            typename CentroidMetadataT>
  void buildPersistence() {
    assert(clock_.get() != nullptr);
    string rockDir = options_->getDataDir() + "/rock";
    UniquePointer<RockHandleIf> rockHandle(new RockHandleT(rockDir));
    UniquePointer<SyncPersistenceIf> syncPersistence(
        new SyncPersistenceT(clock_, std::move(rockHandle)));
    persistence_.reset(
        new PersistenceT(std::move(syncPersistence),
                         make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
                             options_->getRocksDbThreadCount())));
    centroidMetadataDb_.reset(new CentroidMetadataT(persistence_));
  }

  template <typename ClockT>
  void buildClock() {
    shared_ptr<util::ClockIf> clockPtr(new ClockT);
    clock_ = clockPtr;
  }

  template <typename ProcessWorkerT,
            typename ProcessorT,
            typename StemmerT,
            typename StopwordFilterT,
            typename HasherT>
  void buildDocumentProcessor() {
    assert(clock_.get() != nullptr);
    shared_ptr<StemmerIf> stemmer(new StemmerT);
    shared_ptr<StopwordFilterIf> stopwordFilter(new StopwordFilterT);
    shared_ptr<DocumentProcessorIf> processor(
        new ProcessorT(stemmer, stopwordFilter, clock_));
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
        options_->getDocumentProcessingThreadCount());
    shared_ptr<util::Sha1HasherIf> hasher(new HasherT);
    processor_.reset(new ProcessWorkerT(processor, hasher, threadPool));
  }

  template <
    typename CentroidUpdateWorkerT,
    typename CentroidUpdaterFactoryT,
    typename DocumentAccumulatorFactoryT
  >
  void buildCentroidUpdateWorker() {
    assert(persistence_.get() != nullptr);
    assert(clock_.get() != nullptr);

    shared_ptr<DocumentAccumulatorFactoryIf> accumulator(
      new DocumentAccumulatorFactoryT
    );
    shared_ptr<CentroidUpdaterFactoryIf> updaterFactory(
        new CentroidUpdaterFactoryT(persistence_, centroidMetadataDb_, accumulator, clock_));
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
        options_->getCentroidUpdateThreadCount());
    centroidUpdater_.reset(
        new CentroidUpdateWorkerT(updaterFactory, threadPool));
  }

  template <typename SimilarityScoreWorkerT>
  void buildSimilarityWorker() {
    assert(persistence_.get() != nullptr);
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
        options_->getSimilarityScoreThreadCount());
    similarityWorker_.reset(new SimilarityScoreWorkerT(
        persistence_, centroidMetadataDb_, threadPool));
  }

  template <typename RelevanceServerT>
  shared_ptr<RelevanceServerIf> buildServer() {
    assert(clock_.get() != nullptr);
    assert(persistence_.get() != nullptr);
    assert(processor_.get() != nullptr);
    assert(similarityWorker_.get() != nullptr);
    assert(centroidUpdater_.get() != nullptr);
    auto server = make_shared<RelevanceServerT>(
        persistence_, centroidMetadataDb_, clock_, similarityWorker_,
        processor_, centroidUpdater_);
    server->initialize();
    return server;
  }

  template <typename RelevanceServerT>
  shared_ptr<apache::thrift::ThriftServer> buildThriftServer() {
    auto service =
        make_shared<ThriftRelevanceServer>(buildServer<RelevanceServerT>());
    bool allowInsecureLoopback = true;
    string saslPolicy = "";
    auto thriftServer = make_shared<apache::thrift::ThriftServer>(
        saslPolicy, allowInsecureLoopback);
    thriftServer->setInterface(service);
    thriftServer->setTaskExpireTime(options_->getTaskExpireTime());
    thriftServer->setPort(options_->getThriftPort());
    return thriftServer;
  }
};

} // server
} // relevanced
