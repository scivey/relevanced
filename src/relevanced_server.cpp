#include <string>
#include <memory>
#include <chrono>
#include <glog/logging.h>

#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/RockHandle.h"
#include "server/RelevanceServer.h"
#include "server/RelevanceServerOptions.h"
#include "server/ServerBuilder.h"
#include "server/ThriftRelevanceServer.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "stemmer/PorterStemmer.h"
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::document_processing_worker;
using namespace relevanced::persistence;
using namespace relevanced::similarity_score_worker;
using namespace relevanced::server;
using relevanced::stopwords::StopwordFilter;
using relevanced::stemmer::PorterStemmer;
using relevanced::tokenizer::Tokenizer;

int main() {
  thread t1([](){
    LOG(INFO) << "building...";
    auto options = make_shared<RelevanceServerOptions>(
      "data", 8097
    );
    server::ServerBuilder builder(options);
    builder.buildPersistence<
      RockHandle, SyncPersistence, Persistence
    >();
    builder.buildDocumentProcessor<
      DocumentProcessingWorker, DocumentProcessor,
      Tokenizer, PorterStemmer, StopwordFilter
    >();
    builder.buildCentroidUpdateWorker<
      CentroidUpdateWorker, CentroidUpdaterFactory
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
