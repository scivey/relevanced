#include <string>
#include <memory>
#include <chrono>
#include <glog/logging.h>

#include "DocumentProcessor.h"
#include "DocumentProcessingWorker.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/RockHandle.h"
#include "ProcessedDocument.h"
#include "SimilarityScoreWorker.h"
#include "RelevanceServer.h"
#include "RelevanceServerOptions.h"
#include "CentroidUpdateWorker.h"
#include "ServerBuilder.h"
#include "stemmer/PorterStemmer.h"
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "ThriftRelevanceServer.h"
#include "tokenizer/Tokenizer.h"
#include "util.h"

using namespace std;
using namespace persistence;
using stemmer::StemmerIf;
using stemmer::PorterStemmer;
using stopwords::StopwordFilter;
using stopwords::StopwordFilterIf;
using tokenizer::TokenizerIf;
using tokenizer::Tokenizer;

int main() {
  thread t1([](){
    LOG(INFO) << "building...";
    auto options = make_shared<RelevanceServerOptions>(
      "data", 8097
    );
    builders::ServerBuilder builder(options);
    builder.buildPersistence<
      RockHandle, SyncPersistence, Persistence
    >();
    builder.buildDocumentProcessor<
      DocumentProcessingWorker, DocumentProcessor,
      Tokenizer, PorterStemmer, StopwordFilter
    >();
    builder.buildCentroidUpdateWorker<
      CentroidUpdateWorker
    >();
    builder.buildSimilarityWorker<
      SimilarityScoreWorker
    >();
    auto server = builder.buildThriftServer<RelevanceServer>();
    LOG(INFO) << format("listening on port: {}", options->thriftPort);
    server->serve();
  });
  t1.join();
  LOG(INFO) << "end";
}
