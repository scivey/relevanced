#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

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

void RelevanceScoreWorker::initialize() {
  LOG(INFO) << "[ loading... ]";
  auto collectionDb = persistence_->getCollectionDb().lock();
  auto collectionIds = collectionDb->listCollections().get();
  SYNCHRONIZED(centroids_) {
    for (auto &id: collectionIds) {
      LOG(INFO) << "[ loading centroid: " << id << " ... ]";
      ProcessedCentroid *centroid = centroidManager_->getCentroid(id).get();
      centroids_[id] = centroid;
    }
  }
  LOG(INFO) << "[ done loading ]";
}

Future<bool> RelevanceScoreWorker::reloadCentroid(string id) {
  return centroidManager_->getCentroid(id).then([id, this](ProcessedCentroid *centroid) {
    LOG(INFO) << "got processed centroid...";
    assert(centroid != nullptr);
    SYNCHRONIZED(centroids_) {
      centroids_[id] = centroid;
    }
    LOG(INFO) << "inserted";
    return true;
  });
}

Future<double> RelevanceScoreWorker::getRelevanceForDoc(string collectionId, ProcessedDocument *doc) {
  return threadPool_.addFuture([this, collectionId, doc](){
    ProcessedCentroid *center;
    SYNCHRONIZED(centroids_) {
      center = centroids_[collectionId];
    }
    return center->score(doc);
  });
}

Future<double> RelevanceScoreWorker::getRelevanceForDoc(string collectionId, shared_ptr<ProcessedDocument> doc) {
  return getRelevanceForDoc(collectionId, doc.get());
}

Future<double> RelevanceScoreWorker::getRelevanceForText(string collectionId, string text) {
  return threadPool_.addFuture([this, collectionId, text](){
    ProcessedCentroid *center;
    SYNCHRONIZED(centroids_) {
      center = centroids_[collectionId];
    }
    Document doc("no-id", text);
    auto processed = docProcessor_->process(doc);
    return center->score(&processed);
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
