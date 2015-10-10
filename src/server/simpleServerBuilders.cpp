#include <glog/logging.h>
#include "simpleServerBuilders.h"
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
#include "server/ThriftServerWrapper.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "stemmer/PorterStemmer.h"
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "util/util.h"
#include "util/Clock.h"
#include "util/Sha1Hasher.h"

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

namespace relevanced {
namespace server {

shared_ptr<ThriftServerWrapper> buildNormalThriftServer(shared_ptr<RelevanceServerOptions> options) {
  ServerBuilder builder(options);
  builder.buildClock<util::Clock>();
  builder.buildPersistence<
    RockHandle, SyncPersistence, Persistence
  >();
  builder.buildDocumentProcessor<
    DocumentProcessingWorker, DocumentProcessor,
    Tokenizer, PorterStemmer, StopwordFilter, util::Sha1Hasher
  >();
  builder.buildCentroidUpdateWorker<
    CentroidUpdateWorker, CentroidUpdaterFactory
  >();
  builder.buildSimilarityWorker<
    SimilarityScoreWorker
  >();
  auto server = builder.buildThriftServer<RelevanceServer>();
  auto wrapper = std::make_shared<ThriftServerWrapper>(server);
  return wrapper;
}

}
}
