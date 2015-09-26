#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

#include <folly/Optional.h>
#include <folly/Synchronized.h>
#include <vector>
#include <cassert>

#include "persistence/DocumentDB.h"
#include "persistence/CentroidDB.h"
#include "persistence/CollectionDB.h"
#include "CentroidManager.h"
#include "ProcessedCentroid.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "RelevanceScoreWorker.h"
#include "util.h"

using namespace wangle;
using namespace folly;
using namespace std;

RelevanceScoreWorker::RelevanceScoreWorker(
  shared_ptr<persistence::PersistenceServiceIf> persistence,
  shared_ptr<CentroidManager> centroidManager,
  shared_ptr<DocumentProcessor> docProcessor
): persistence_(persistence),
   centroidManager_(centroidManager),
   docProcessor_(docProcessor){}

// run synchronously on startup
void RelevanceScoreWorker::initialize() {
  LOG(INFO) << "[ loading... ]";
  auto collectionDb = persistence_->getCollectionDb().lock();
  auto collectionIds = collectionDb->listCollections().get();
  SYNCHRONIZED(centroids_) {
    for (auto &id: collectionIds) {
      LOG(INFO) << "[ loading centroid: " << id << " ... ]";
      auto centroid = centroidManager_->getCentroid(id).get();
      if (centroid.hasValue()) {
        centroids_[id] = centroid.value();
      } else {
        LOG(INFO) << "no existing centroid for collection " << id;
      }
    }
  }
  centroidManager_->onUpdate([this](const string &id) {
    reloadCentroid(id);
  });
  LOG(INFO) << "[ done loading ]";
}

Future<bool> RelevanceScoreWorker::reloadCentroid(string id) {
  return centroidManager_->getCentroid(id).then([id, this](Optional<shared_ptr<ProcessedCentroid>> centroid) {
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

Optional<shared_ptr<ProcessedCentroid>> RelevanceScoreWorker::getLoadedCentroid_(const string &id) {
  Optional<shared_ptr<ProcessedCentroid>> center;
  SYNCHRONIZED(centroids_) {
    auto elem = centroids_.find(id);
    if (elem != centroids_.end()) {
      center.assign(elem->second);
    }
  }
  return center;
}

Future<double> RelevanceScoreWorker::getRelevanceForDoc(string collectionId, ProcessedDocument *doc) {
  return threadPool_.addFuture([this, collectionId, doc](){
    auto centroid = getLoadedCentroid_(collectionId);
    if (!centroid.hasValue()) {
      LOG(INFO) << "relevance request against null centroid: " << collectionId;
      return 0.0;
    }
    return centroid.value()->score(doc);
  });
}

Future<double> RelevanceScoreWorker::getRelevanceForDoc(string collectionId, shared_ptr<ProcessedDocument> doc) {
  return getRelevanceForDoc(collectionId, doc.get());
}

Future<double> RelevanceScoreWorker::getRelevanceForText(string collectionId, string text) {
  return threadPool_.addFuture([this, collectionId, text](){
    auto centroid = getLoadedCentroid_(collectionId);
    if (!centroid.hasValue()) {
      LOG(INFO) << "relevance request against null centroid: " << collectionId;
      return 0.0;
    }
    Document doc("no-id", text);
    auto processed = docProcessor_->process(doc);
    return centroid.value()->score(&processed);
  });
}

Future<bool> RelevanceScoreWorker::recompute(string collectionId) {
  return centroidManager_->update(collectionId).then([this, collectionId] (bool updated) {
    LOG(INFO) << "updated ? " << updated;
    if (updated) {
      return reloadCentroid(collectionId);
    }
    return makeFuture(false);
  });
}

void RelevanceScoreWorker::triggerUpdate(string collectionId) {
  centroidManager_->triggerUpdate(collectionId);
}
