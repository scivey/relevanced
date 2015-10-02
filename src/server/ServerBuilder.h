#pragma once
#include <cassert>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "stemmer/StemmerIf.h"
#include "stemmer/PorterStemmer.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/RockHandle.h"
#include "models/ProcessedDocument.h"
#include "serialization/serializers.h"
#include "server/RelevanceServer.h"
#include "server/ThriftRelevanceServer.h"
#include "util/util.h"
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
using relevanced::stemmer::PorterStemmer;
using relevanced::stopwords::StopwordFilter;
using relevanced::stopwords::StopwordFilterIf;
using relevanced::tokenizer::TokenizerIf;
using relevanced::tokenizer::Tokenizer;
using wangle::CPUThreadPoolExecutor;
using wangle::FutureExecutor;
using util::UniquePointer;

/**
 * This is an AbstractNonsenseArchitectureAstronautFactoryManager.
 * 
 * There is some indirection in the rest of the code for the sake of
 * testability.  For instance, the `DocumentProcessor` and `CentroidUpdater` are
 * defined in terms of instances even though the "real" `RelevanceServer`
 * only needs one implementation of each.
 * 
 * `ServerBuilder`'s job is to stitch those layers of indirection together into
 * a usable `RelevancedServer` or `ThriftRelevanceServer` instance.
 * 
 * The main goal here is to contain the abstract nonsense to one place.
 */
class ServerBuilder {
  shared_ptr<PersistenceIf> persistence_;
  shared_ptr<DocumentProcessingWorkerIf> processor_;
  shared_ptr<SimilarityScoreWorkerIf> similarityWorker_;
  shared_ptr<CentroidUpdateWorkerIf> centroidUpdater_;
  shared_ptr<RelevanceServerOptions> options_;
public:
  ServerBuilder(shared_ptr<RelevanceServerOptions> options): options_(options) {}

  template<
    typename RockHandleT,
    typename SyncPersistenceT,
    typename PersistenceT
  >
  void buildPersistence() {
    string rockDir = options_->dataDir + "/rock";
    UniquePointer<RockHandleIf> rockHandle(
      new RockHandleT(rockDir)
    );
    UniquePointer<SyncPersistenceIf> syncPersistence(
      new SyncPersistenceT(std::move(rockHandle))
    );
    persistence_.reset(new PersistenceT(
        std::move(syncPersistence),
        make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
          options_->getRocksDbThreadCount()
        )
    ));
  }

  template<typename ProcessWorkerT, typename ProcessorT, typename TokenizerT, typename StemmerT, typename StopwordFilterT>
  void buildDocumentProcessor() {
    shared_ptr<TokenizerIf> tokenizer(new TokenizerT);
    shared_ptr<StemmerIf> stemmer(new StemmerT);
    shared_ptr<StopwordFilterIf> stopwordFilter(new StopwordFilterT);
    shared_ptr<DocumentProcessorIf> processor(new ProcessorT(
        tokenizer, stemmer, stopwordFilter
    ));
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
      options_->getDocumentProcessingThreadCount()
    );
    processor_.reset(new ProcessWorkerT(
      processor, threadPool
    ));
  }

  template<typename CentroidUpdateWorkerT, typename CentroidUpdaterFactoryT>
  void buildCentroidUpdateWorker() {
    assert(persistence_.get() != nullptr);
    shared_ptr<CentroidUpdaterFactoryIf> updaterFactory(
      new CentroidUpdaterFactoryT(persistence_)
    );
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
      options_->getCentroidUpdateThreadCount()
    );
    centroidUpdater_.reset(new CentroidUpdateWorkerT(
      updaterFactory, threadPool
    ));
  }

  template<typename SimilarityScoreWorkerT>
  void buildSimilarityWorker() {
    assert(persistence_.get() != nullptr);
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
      options_->getSimilarityScoreThreadCount()
    );
    similarityWorker_.reset(
      new SimilarityScoreWorkerT(persistence_, threadPool)
    );
  }

  template<typename RelevanceServerT>
  shared_ptr<RelevanceServerIf> buildServer(){
    assert(persistence_.get() != nullptr);
    assert(processor_.get() != nullptr);
    assert(similarityWorker_.get() != nullptr);
    assert(centroidUpdater_.get() != nullptr);
    auto server = make_shared<RelevanceServerT>(
      persistence_, similarityWorker_, processor_, centroidUpdater_
    );
    server->initialize();
    return server;
  }

  template<typename RelevanceServerT>
  shared_ptr<apache::thrift::ThriftServer> buildThriftServer(){
    auto service = make_shared<ThriftRelevanceServer>(
      buildServer<RelevanceServerT>()
    );
    bool allowInsecureLoopback = true;
    string saslPolicy = "";
    auto thriftServer = make_shared<apache::thrift::ThriftServer>(
      saslPolicy, allowInsecureLoopback
    );
    thriftServer->setInterface(service);
    thriftServer->setTaskExpireTime(options_->taskExpireTime);
    thriftServer->setPort(options_->thriftPort);
    return thriftServer;
  }

};

} // server
} // relevanced
