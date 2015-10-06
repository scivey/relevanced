#include <string>
#include <memory>
#include <chrono>
#include <glog/logging.h>
#include <gflags/gflags.h>

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

DEFINE_bool(integration_test_mode, false, "Enable dangerous commands used for testing (e.g. erase everything)");
DEFINE_int32(port, 8097, "Port for relevanced's Thrift server to listen on");
DEFINE_string(data_dir, "./data", "Directory where the server should place its RocksDB database");
DEFINE_int32(rocks_db_threads, 8, "Number of threads to spawn in the RocksDB thread pool");
DEFINE_int32(centroid_update_threads, 4, "Number of threads in the centroid updating pool");
DEFINE_int32(similarity_score_threads, 4, "Number of threads in the similarity scoring pool");
DEFINE_int32(document_processing_threads, 4, "Number of threads in the document processing pool");

int main(int argc, char **argv) {
  google::SetUsageMessage("Usage");
  google::ParseCommandLineFlags(&argc, &argv, true);
  thread t1([](){
    auto options = make_shared<RelevanceServerOptions>();
    options->setDataDir(FLAGS_data_dir);
    options->setThriftPort(FLAGS_port);
    options->setIntegrationTestMode(FLAGS_integration_test_mode);
    options->setRocksDbThreadCount(FLAGS_rocks_db_threads);
    options->setCentroidUpdateThreadCount(FLAGS_centroid_update_threads);
    options->setSimilarityScoreThreadCount(FLAGS_similarity_score_threads);
    options->setDocumentProcessingThreadCount(FLAGS_document_processing_threads);
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
    LOG(INFO) << format("listening on port: {}", options->getThriftPort());
    server->serve();
  });
  t1.join();
}
