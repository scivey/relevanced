#include <string>
#include <memory>
#include <chrono>

#include <glog/logging.h>
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "stemmer/StemmerIf.h"
#include "stemmer/PorterStemmer.h"
#include "DocumentProcessor.h"
#include "RelevanceServerOptions.h"
#include "persistence/PersistenceService.h"
#include "persistence/RockHandle.h"
#include "persistence/ColonPrefixedRockHandle.h"
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
#include "ServerBuilder.h"
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

using namespace std;
using namespace persistence;
using stemmer::StemmerIf;
using stemmer::PorterStemmer;
using stopwords::StopwordFilter;
using stopwords::StopwordFilterIf;
using tokenizer::TokenizerIf;
using tokenizer::Tokenizer;

int main() {
  LOG(INFO) << "start";
  thread t1([](){
    auto options = make_shared<RelevanceServerOptions>(
      "data", 8097
    );
    builders::ServerBuilder builder(options);
    builder.buildPersistence<
      RockHandle, CentroidDBHandle, CentroidDB,
      CollectionDBHandle, CollectionDB, DocumentDBHandle,
      DocumentDB, PersistenceService
    >();
    builder.buildDocumentProcessor<
      DocumentProcessor, Tokenizer,
      PorterStemmer, StopwordFilter
    >();
    builder.buildRelevanceWorker<
      RelevanceScoreWorker, CentroidManager, CentroidUpdater
    >();
    auto server = builder.buildThriftServer<RelevanceServer>();
    LOG(INFO) << "listening on: " << options->thriftPort;
    server->serve();
  });
  t1.join();
  LOG(INFO) << "end";
}
