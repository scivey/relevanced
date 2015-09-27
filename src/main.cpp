#include <string>
#include <memory>
#include <chrono>

#include <glog/logging.h>

#include "DocumentProcessor.h"
#include "DocumentProcessingWorker.h"

#include "persistence/CentroidDB.h"
#include "persistence/CentroidDBHandle.h"
#include "persistence/ClassifierDB.h"
#include "persistence/ClassifierDBHandle.h"
#include "persistence/DocumentDB.h"
#include "persistence/DocumentDBHandle.h"
#include "persistence/PersistenceService.h"
#include "persistence/PrefixedRockHandle.h"
#include "persistence/RockHandle.h"

#include "ProcessedDocument.h"
#include "RelevanceScoreWorker.h"
#include "RelevanceServer.h"
#include "RelevanceServerOptions.h"
#include "serialization/serializers.h"
#include "ServerBuilder.h"
#include "stemmer/PorterStemmer.h"
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "ThriftRelevanceServer.h"
#include "tokenizer/Tokenizer.h"
#include "util.h"

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
      RockHandle, PrefixedRockHandle, CentroidDBHandle, CentroidDB,
      ClassifierDBHandle, ClassifierDB, DocumentDBHandle,
      DocumentDB, PersistenceService
    >();
    builder.buildDocumentProcessor<
      DocumentProcessingWorker, DocumentProcessor,
      Tokenizer, PorterStemmer, StopwordFilter
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
