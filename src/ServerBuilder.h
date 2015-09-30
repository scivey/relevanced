#pragma once
#include <cassert>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "stemmer/StemmerIf.h"
#include "stemmer/PorterStemmer.h"
#include "DocumentProcessor.h"
#include "DocumentProcessingWorker.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/RockHandle.h"
#include "persistence/PrefixedRockHandle.h"
#include "ProcessedDocument.h"
#include "serialization/serializers.h"
#include "RelevanceServer.h"
#include "ThriftRelevanceServer.h"
#include "util.h"
#include "RelevanceScoreWorker.h"
#include "RelevanceServerOptions.h"

using namespace std;
using util::UniquePointer;
using namespace std;
using namespace folly;
using namespace persistence;
using stemmer::StemmerIf;
using stemmer::PorterStemmer;
using stopwords::StopwordFilter;
using stopwords::StopwordFilterIf;
using tokenizer::TokenizerIf;
using tokenizer::Tokenizer;
using wangle::CPUThreadPoolExecutor;
using wangle::FutureExecutor;
using util::UniquePointer;

namespace builders {

class ServerBuilder {
  shared_ptr<PersistenceIf> persistence_;
  shared_ptr<DocumentProcessingWorkerIf> processor_;
  shared_ptr<RelevanceScoreWorkerIf> relevanceWorker_;
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
      (RockHandleIf*) new RockHandleT(rockDir)
    );
    UniquePointer<SyncPersistenceIf> syncPersistence(
      (SyncPersistenceIf*) new SyncPersistenceT(std::move(rockHandle))
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
    shared_ptr<TokenizerIf> tokenizer(
      (TokenizerIf*) new TokenizerT
    );
    shared_ptr<StemmerIf> stemmer(
      (StemmerIf*) new StemmerT
    );
    shared_ptr<StopwordFilterIf> stopwordFilter(
      (StopwordFilterIf*) new StopwordFilterT
    );
    shared_ptr<DocumentProcessorIf> processor(
      (DocumentProcessorIf*) new ProcessorT(
        tokenizer, stemmer, stopwordFilter
      )
    );
    processor_.reset(new ProcessWorkerT(processor));
  }

  template<typename CentroidUpdateWorkerT>
  void buildCentroidUpdateWorker() {
    assert(persistence_.get() != nullptr);
    auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(
      options_->getCentroidUpdateThreadCount()
    );
    centroidUpdater_.reset(new CentroidUpdateWorkerT(
      persistence_, threadPool
    ));
  }

  template<typename RelevanceScoreWorkerT>
  void buildRelevanceWorker() {
    assert(persistence_.get() != nullptr);
    relevanceWorker_.reset(
      new RelevanceScoreWorkerT(persistence_)
    );
  }

  template<typename RelevanceServerT>
  shared_ptr<RelevanceServerIf> buildServer(){
    assert(persistence_.get() != nullptr);
    assert(processor_.get() != nullptr);
    assert(relevanceWorker_.get() != nullptr);
    assert(centroidUpdater_.get() != nullptr);
    auto server = make_shared<RelevanceServerT>(
      persistence_, relevanceWorker_, processor_, centroidUpdater_
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


} // builders