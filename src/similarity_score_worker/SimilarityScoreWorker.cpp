#include <cassert>
#include <memory>
#include <vector>

#include <folly/ExceptionWrapper.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <folly/Synchronized.h>
#include <folly/Format.h>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "models/Centroid.h"
#include "models/ProcessedDocument.h"
#include "models/WordVector.h"
#include "persistence/CentroidMetadataDb.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "util/util.h"
#include "util/ConcurrentMap.h"

namespace relevanced {
namespace similarity_score_worker {

using models::WordVector;
using models::ProcessedDocument;
using models::Centroid;
using util::ConcurrentMap;

using thrift_protocol::ECentroidDoesNotExist;
using namespace wangle;
using namespace folly;
using namespace std;

SimilarityScoreWorker::SimilarityScoreWorker(
    shared_ptr<persistence::PersistenceIf> persistence,
    shared_ptr<persistence::CentroidMetadataDbIf> centroidMetadataDb,
    shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
    : persistence_(persistence),
      centroidMetadataDb_(centroidMetadataDb),
      threadPool_(threadPool) {
        centroids_ = std::move(folly::make_unique<ConcurrentMap<string, Centroid>>(10));
      }

// run synchronously on startup
void SimilarityScoreWorker::initialize() {
  LOG(INFO) << "SimilarityScoreWorker initializing...";
  auto centroidIds = persistence_->listAllCentroids().get();
  for (auto &id : centroidIds) {
    auto centroid = persistence_->loadCentroidUniqueOption(id).get();
    if (centroid.hasValue()) {
      centroids_->insertOrUpdate(id, std::move(centroid.value()));
    } else {
      LOG(INFO) << format("centroid '{}' doesn't seem to exist...", id);
    }
  }
  LOG(INFO) << "SimilarityScoreWorker initialized.";
}

Future<bool> SimilarityScoreWorker::reloadCentroid(string id) {
  return persistence_->loadCentroidUniqueOption(id)
      .then([id, this](Optional<unique_ptr<Centroid>> centroid) {
        if (!centroid.hasValue()) {
          LOG(INFO) << format("tried to reload null centroid '{}'", id);
          return false;
        }
        centroids_->insertOrUpdate(id, std::move(centroid.value()));
        LOG(INFO) << format("inserted reloaded centroid '{}'", id);
        return true;
      });
}


Future<Try<double>> SimilarityScoreWorker::getDocumentSimilarity(
    string centroidId, ProcessedDocument *doc) {
  return threadPool_->addFuture([this, centroidId, doc]() {
    auto centroid = centroids_->getOption(centroidId);
    if (!centroid.hasValue()) {
      LOG(INFO) << "relevance request against null centroid: " << centroidId;
      return Try<double>(make_exception_wrapper<ECentroidDoesNotExist>());
    }
    auto result = centroid.value()->score(doc);
    LOG(INFO) << format("relevance vs centroid '{}' : {}", centroidId, result);
    return Try<double>(result);
  });
}

Future<Try<double>> SimilarityScoreWorker::getDocumentSimilarity(
    string centroidId, shared_ptr<ProcessedDocument> doc) {
  return getDocumentSimilarity(centroidId, doc.get());
}

Future<Try<double>> SimilarityScoreWorker::getCentroidSimilarity(
    string centroid1Id, string centroid2Id) {
  return threadPool_->addFuture([this, centroid1Id, centroid2Id]() {
    auto centroid1 = centroids_->getOption(centroid1Id);
    auto centroid2 = centroids_->getOption(centroid2Id);
    if (!centroid1.hasValue() || !centroid2.hasValue()) {
      return Try<double>(make_exception_wrapper<ECentroidDoesNotExist>());
    }
    return Try<double>(
        centroid1.value()->score(&centroid2.value().get()->wordVector));
  });
}

} // similarity_score_worker
} // relevanced
