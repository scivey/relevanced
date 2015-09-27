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
#include "persistence/PersistenceService.h"
#include "persistence/RockHandle.h"
#include "persistence/PrefixedRockHandle.h"
#include "ProcessedDocument.h"
#include "serialization/serializers.h"
#include "persistence/CollectionDB.h"
#include "persistence/CollectionDBHandle.h"
#include "RelevanceServer.h"
#include "ThriftRelevanceServer.h"
#include "persistence/DocumentDB.h"
#include "persistence/DocumentDBHandle.h"
#include "persistence/CentroidDB.h"
#include "persistence/CentroidDBHandle.h"
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

template<typename RockHandleT,
  typename PrefixedRockT,
  typename CentroidDBHandleT,
  typename CentroidDBT,
  typename CollectionDBHandleT,
  typename CollectionDBT,
  typename DocumentDBHandleT,
  typename DocumentDBT,
  typename PersistenceServiceT
>
shared_ptr<PersistenceServiceIf> buildPersistence(const string &dataDir) {

  auto threadPool = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(4);
  string centroidDir = dataDir + "/centroids";
  UniquePointer<RockHandleIf> centroidRock(
    (RockHandleIf*) new RockHandleT(centroidDir)
  );
  UniquePointer<CentroidDBHandleIf> centroidDbHandle(
    (CentroidDBHandleIf*) new CentroidDBHandleT(std::move(centroidRock))
  );
  shared_ptr<CentroidDBIf> centroidDb(
    (CentroidDBIf*) new CentroidDBT(
      std::move(centroidDbHandle),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(1)
    )
  );

  string docDir = dataDir + "/documents";
  UniquePointer<RockHandleIf> docRock(
    (RockHandleIf*) new RockHandleT(docDir)
  );
  UniquePointer<DocumentDBHandleIf> docDbHandle(
    (DocumentDBHandleIf*) new DocumentDBHandleT(std::move(docRock))
  );
  shared_ptr<DocumentDBIf> docDb(
    (DocumentDBIf*) new DocumentDBT(
      std::move(docDbHandle),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(4)
    )
  );

  string collectionDir = dataDir + "/collections_rock";
  UniquePointer<RockHandleIf> collectionListRock(
    (RockHandleIf*) new RockHandleT(collectionDir)
  );
  string collectionDocsDir = dataDir + "/collection_docs_rock";
  UniquePointer<RockHandleIf> collectionDocumentsRock(
    new PrefixedRockT(collectionDocsDir)
  );
  UniquePointer<CollectionDBHandleIf> collDbHandle(
    (CollectionDBHandleIf*) new CollectionDBHandleT(
      std::move(collectionDocumentsRock), std::move(collectionListRock)
    )
  );
  shared_ptr<CollectionDBIf> collDb(
    (CollectionDBIf*) new CollectionDBT(
      std::move(collDbHandle),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(4)
    )
  );
  collDb->initialize();

  shared_ptr<PersistenceServiceIf> persistence(
    (PersistenceServiceIf*) new PersistenceServiceT(
      centroidDb, docDb, collDb
    )
  );
  return std::move(persistence);
}

template<typename CentroidManagerT, typename CentroidUpdaterT>
shared_ptr<CentroidManagerIf> buildCentroidManager(shared_ptr<PersistenceServiceIf> persistence) {
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

template<typename ProcessorT, typename TokenizerT, typename StemmerT, typename StopwordFilterT>
shared_ptr<DocumentProcessorIf> buildDocumentProcessor() {
  shared_ptr<TokenizerIf> tokenizer(
    (TokenizerIf*) new TokenizerT
  );
  shared_ptr<StemmerIf> stemmer(
    (StemmerIf*) new StemmerT
  );
  shared_ptr<StopwordFilterIf> stopwordFilter(
    (StopwordFilterIf*) new StopwordFilterT
  );
  return shared_ptr<DocumentProcessorIf>(
    (DocumentProcessorIf*) new ProcessorT(
      tokenizer, stemmer, stopwordFilter
    )
  );
}

template<typename WorkerType>
shared_ptr<RelevanceScoreWorkerIf> buildRelevanceWorker(
  shared_ptr<PersistenceServiceIf> persistence,
  shared_ptr<CentroidManagerIf> manager,
  shared_ptr<DocumentProcessorIf> processor
) {
  shared_ptr<RelevanceScoreWorkerIf> result(
    (RelevanceScoreWorkerIf*) new WorkerType(
      persistence, manager, processor
    )
  );
  result->initialize();
  return result;
}

} // detail

class ServerBuilder {
  shared_ptr<PersistenceServiceIf> persistence_;
  shared_ptr<DocumentProcessorIf> processor_;
  shared_ptr<RelevanceScoreWorkerIf> relevanceWorker_;
  shared_ptr<CentroidManagerIf> centroidManager_;
  shared_ptr<RelevanceServerOptions> options_;
public:
  ServerBuilder(shared_ptr<RelevanceServerOptions> options): options_(options) {}

  template<typename RockHandleT,
    typename PrefixedRockT,
    typename CentroidDBHandleT,
    typename CentroidDBT,
    typename CollectionDBHandleT,
    typename CollectionDBT,
    typename DocumentDBHandleT,
    typename DocumentDBT,
    typename PersistenceServiceT
  >
  void buildPersistence() {
    persistence_ = detail::buildPersistence<
      RockHandleT, PrefixedRockT, CentroidDBHandleT, CentroidDBT,
      CollectionDBHandleT, CollectionDBT,
      DocumentDBHandleT, DocumentDBT, PersistenceServiceT
    >(options_->dataDir);
  }

  template<typename ProcessorT, typename TokenizerT, typename StemmerT, typename StopwordFilterT>
  void buildDocumentProcessor() {
    processor_ = detail::buildDocumentProcessor<
      ProcessorT, TokenizerT, StemmerT, StopwordFilterT
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
    assert(processor_.get() != nullptr);
    centroidManager_ = detail::buildCentroidManager<
      CentroidManagerT, CentroidUpdaterT
    >(persistence_);
    relevanceWorker_ = detail::buildRelevanceWorker<RelevanceScoreWorkerT>(
      persistence_, centroidManager_, processor_
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