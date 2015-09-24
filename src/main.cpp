#include <string>
#include <memory>
#include <chrono>

#include <glog/logging.h>
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "stemmer/StemmerIf.h"
#include "stemmer/PorterStemmer.h"
#include "DocumentProcessor.h"
#include "RelevanceServer.h"
#include "persistence/PersistenceService.h"
#include "persistence/SqlDb.h"
#include "persistence/RockHandle.h"
#include "persistence/CollectionDB.h"
#include "persistence/CollectionDBHandle.h"
#include "persistence/DocumentDB.h"
#include "persistence/DocumentDBHandle.h"
#include "persistence/CentroidDB.h"
#include "persistence/CentroidDBHandle.h"
#include "util.h"
#include "RelevanceScoreWorker.h"
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

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

shared_ptr<PersistenceServiceIf> getPersistence() {

  auto threadPool = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(4);

  UniquePointer<RockHandleIf> centroidRock(
    (RockHandleIf*) new RockHandle("data/centroids")
  );
  UniquePointer<CentroidDBHandleIf> centroidDbHandle(
    (CentroidDBHandleIf*) new CentroidDBHandle(std::move(centroidRock))
  );
  shared_ptr<CentroidDBIf> centroidDb(
    (CentroidDBIf*) new CentroidDB(
      std::move(centroidDbHandle),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(1)
    )
  );

  UniquePointer<RockHandleIf> docRock(
    (RockHandleIf*) new RockHandle("data/documents")
  );
  UniquePointer<DocumentDBHandleIf> docDbHandle(
    (DocumentDBHandleIf*) new DocumentDBHandle(std::move(docRock))
  );
  shared_ptr<DocumentDBIf> docDb(
    (DocumentDBIf*) new DocumentDB(
      std::move(docDbHandle),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(1)
    )
  );

  UniquePointer<SqlDb> sqlDb(
    new SqlDb("data/collections.sqlite")
  );
  UniquePointer<CollectionDBHandleIf> collDbHandle(
    (CollectionDBHandleIf*) new CollectionDBHandle(std::move(sqlDb))
  );
  collDbHandle->ensureTables();
  shared_ptr<CollectionDBIf> collDb(
    (CollectionDBIf*) new CollectionDB(
      std::move(collDbHandle),
      make_shared<FutureExecutor<CPUThreadPoolExecutor>>(1)
    )
  );

  shared_ptr<PersistenceServiceIf> persistence(
    (PersistenceServiceIf*) new PersistenceService(
      centroidDb, docDb, collDb
    )
  );
  return std::move(persistence);
}

shared_ptr<CentroidManager> getCentroidManager(shared_ptr<PersistenceServiceIf> persistence) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  UniquePointer<CentroidUpdater> updater(
    new CentroidUpdater(
      persistence,
      threadPool
    )
  );
  return make_shared<CentroidManager>(
    std::move(updater), persistence
  );
}

int main() {
  LOG(INFO) << "start";
  thread t1([](){
    shared_ptr<TokenizerIf> tokenizer(
      (TokenizerIf*) new Tokenizer
    );
    shared_ptr<StemmerIf> stemmer(
      (StemmerIf*) new PorterStemmer
    );
    shared_ptr<StopwordFilterIf> stopwordFilter(
      (StopwordFilterIf*) new StopwordFilter
    );
    auto documentProcessor = make_shared<DocumentProcessor>(
      stemmer,
      tokenizer,
      stopwordFilter
    );
    auto persistence = getPersistence();
    auto centroidManager = getCentroidManager(persistence);

    auto relevanceWorker = make_shared<RelevanceScoreWorker>(
      persistence, centroidManager, documentProcessor
    );
    relevanceWorker->initialize();
    auto service = make_shared<RelevanceServer>(
      relevanceWorker, documentProcessor, persistence
    );

    bool allowInsecureLoopback = true;
    string saslPolicy = "";
    auto server = new apache::thrift::ThriftServer(
      saslPolicy, allowInsecureLoopback
    );
    server->setInterface(service);
    server->setTaskExpireTime(chrono::milliseconds(60000));
    auto port = 8097;
    server->setPort(port);
    LOG(INFO) << "listening on: " << port;
    server->serve();
  });
  t1.join();
  LOG(INFO) << "end";
}