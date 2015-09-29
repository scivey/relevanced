#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

#include <folly/Optional.h>
#include <folly/Synchronized.h>
#include <vector>
#include <cassert>

#include "persistence/Persistence.h"
#include "CentroidManager.h"
#include "Centroid.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "RelevanceScoreWorker.h"
#include "util.h"

using namespace wangle;
using namespace folly;
using namespace std;

RelevanceScoreWorker::RelevanceScoreWorker(
  shared_ptr<persistence::PersistenceIf> persistence,
  shared_ptr<CentroidManagerIf> centroidManager
): persistence_(persistence),
   centroidManager_(centroidManager){}

// run synchronously on startup
void RelevanceScoreWorker::initialize() {
  LOG(INFO) << "[ loading... ]";
  auto centroidIds = persistence_->listAllCentroids().get();
  SYNCHRONIZED(centroids_) {
    for (auto &id: centroidIds) {
      LOG(INFO) << "[ loading centroid: " << id << " ... ]";
      auto centroid = centroidManager_->getCentroid(id).get();
      if (centroid.hasValue()) {
        centroids_[id] = centroid.value();
      } else {
        LOG(INFO) << "no existing centroid for classifier " << id;
      }
    }
  }
  centroidManager_->onUpdate([this](const string &id) {
    reloadCentroid(id);
  });
  LOG(INFO) << "[ done loading ]";
}

Future<bool> RelevanceScoreWorker::reloadCentroid(string id) {
  return centroidManager_->getCentroid(id).then([id, this](Optional<shared_ptr<Centroid>> centroid) {
    if (!centroid.hasValue()) {
      LOG(INFO) << "tried to reload null centroid: " << id;
      return false;
    }
    SYNCHRONIZED(centroids_) {
      centroids_[id] = centroid.value();
    }
    LOG(INFO) << "inserted";
    return true;
  });
}

Optional<shared_ptr<Centroid>> RelevanceScoreWorker::getLoadedCentroid_(const string &id) {
  Optional<shared_ptr<Centroid>> center;
  SYNCHRONIZED(centroids_) {
    auto elem = centroids_.find(id);
    if (elem != centroids_.end()) {
      center.assign(elem->second);
    }
  }
  return center;
}

Future<double> RelevanceScoreWorker::getDocumentSimilarity(string centroidId, ProcessedDocument *doc) {
  return threadPool_.addFuture([this, centroidId, doc](){
    auto centroid = getLoadedCentroid_(centroidId);
    if (!centroid.hasValue()) {
      LOG(INFO) << "relevance request against null centroid: " << centroidId;
      return 0.0;
    }
    return centroid.value()->score(doc);
  });
}

Future<double> RelevanceScoreWorker::getDocumentSimilarity(string centroidId, shared_ptr<ProcessedDocument> doc) {
  return getDocumentSimilarity(centroidId, doc.get());
}

Future<bool> RelevanceScoreWorker::recomputeCentroid(string centroidId) {
  return centroidManager_->update(centroidId).then([this, centroidId] (bool updated) {
    LOG(INFO) << "updated ? " << updated;
    if (updated) {
      return reloadCentroid(centroidId);
    }
    return makeFuture(false);
  });
}

void RelevanceScoreWorker::triggerUpdate(string centroidId) {
  centroidManager_->triggerUpdate(centroidId);
}
