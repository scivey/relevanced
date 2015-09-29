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


namespace detail {

template<
  typename RockHandleT,
  typename SyncPersistenceT,
  typename PersistenceT
>
shared_ptr<PersistenceIf> buildPersistence(const string &dataDir) {

  string rockDir = dataDir + "/rock";
  UniquePointer<RockHandleIf> rockHandle(
    (RockHandleIf*) new RockHandleT(rockDir)
  );
  UniquePointer<SyncPersistenceIf> syncPersistence(
    (SyncPersistenceIf*) new SyncPersistenceT(std::move(rockHandle))
  );
  shared_ptr<PersistenceIf> persistence(
    (PersistenceIf*) new PersistenceT(
      std::move(syncPersistence),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(8)
    )
  );
  persistence->initialize();
  return persistence;
}

template<typename CentroidManagerT, typename CentroidUpdaterT>
shared_ptr<CentroidManagerIf> buildCentroidManager(shared_ptr<PersistenceIf> persistence) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  UniquePointer<CentroidUpdaterIf> updater(
    (CentroidUpdaterIf*) new CentroidUpdaterT(
      persistence,
      threadPool
    )
  );
  updater->initialize();
  return shared_ptr<CentroidManagerIf>(
    (CentroidManagerIf*) new CentroidManagerT(std::move(updater), persistence)
  );
}

template<typename ProcessWorkerT, typename ProcessorT, typename TokenizerT, typename StemmerT, typename StopwordFilterT>
shared_ptr<DocumentProcessingWorkerIf> buildDocumentProcessor() {
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
  return shared_ptr<DocumentProcessingWorkerIf>(
    new ProcessWorkerT(processor)
  );
}

template<typename WorkerType>
shared_ptr<RelevanceScoreWorkerIf> buildRelevanceWorker(
  shared_ptr<PersistenceIf> persistence,
  shared_ptr<CentroidManagerIf> manager
) {
  shared_ptr<RelevanceScoreWorkerIf> result(
    (RelevanceScoreWorkerIf*) new WorkerType(
      persistence, manager
    )
  );
  result->initialize();
  return result;
}

} // detail

class ServerBuilder {
  shared_ptr<PersistenceIf> persistence_;
  shared_ptr<DocumentProcessingWorkerIf> processor_;
  shared_ptr<RelevanceScoreWorkerIf> relevanceWorker_;
  shared_ptr<CentroidManagerIf> centroidManager_;
  shared_ptr<RelevanceServerOptions> options_;
public:
  ServerBuilder(shared_ptr<RelevanceServerOptions> options): options_(options) {}

  template<
    typename RockHandleT,
    typename SyncPersistenceT,
    typename PersistenceT
  >
  void buildPersistence() {
    persistence_ = detail::buildPersistence<
      RockHandleT, SyncPersistenceT, PersistenceT
    >(options_->dataDir);
  }

  template<typename ProcessWorkerT, typename ProcessorT, typename TokenizerT, typename StemmerT, typename StopwordFilterT>
  void buildDocumentProcessor() {
    processor_ = detail::buildDocumentProcessor<
      ProcessWorkerT, ProcessorT, TokenizerT, StemmerT, StopwordFilterT
    >();
  }

  template<typename CentroidManagerT, typename CentroidUpdaterT>
  void buildCentroidManager() {
    assert(persistence_.get() != nullptr);
    centroidManager_ = detail::buildCentroidManager<
      CentroidManagerT, CentroidUpdaterT
    >(persistence_);
  }

  template<typename RelevanceScoreWorkerT, typename CentroidManagerT, typename CentroidUpdaterT>
  void buildRelevanceWorker() {
    assert(persistence_.get() != nullptr);
    centroidManager_ = detail::buildCentroidManager<
      CentroidManagerT, CentroidUpdaterT
    >(persistence_);
    relevanceWorker_ = detail::buildRelevanceWorker<RelevanceScoreWorkerT>(
      persistence_, centroidManager_
    );
  }

  template<typename RelevanceServerT>
  shared_ptr<RelevanceServerIf> buildServer(){
    assert(persistence_.get() != nullptr);
    assert(processor_.get() != nullptr);
    assert(relevanceWorker_.get() != nullptr);
    return make_shared<RelevanceServerT>(
      relevanceWorker_, processor_, persistence_
    );
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