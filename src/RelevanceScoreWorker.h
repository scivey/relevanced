#pragma once

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/Synchronized.h>
#include <vector>
#include "persistence/DocumentDB.h"
#include "persistence/CentroidDB.h"
#include "persistence/CollectionDB.h"
#include "CentroidManager.h"
#include "ProcessedCentroid.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"

namespace {
  using namespace wangle;
  using namespace folly;
  using namespace std;
}
class RelevanceScoreWorker {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  std::shared_ptr<CentroidManager> centroidManager_;
  shared_ptr<DocumentProcessor> docProcessor_;
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {4};
  folly::Synchronized<std::map<std::string, ProcessedCentroid*>> centroids_;
public:
  RelevanceScoreWorker(
    shared_ptr<persistence::PersistenceServiceIf> persistence,
    shared_ptr<CentroidManager> centroidManager,
    shared_ptr<DocumentProcessor> docProcessor
  ): persistence_(persistence),
     centroidManager_(centroidManager),
     docProcessor_(docProcessor){}

  void initialize() {
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

  Future<bool> reloadCentroid(string id) {
    return centroidManager_->getCentroid(id).then([id, this](ProcessedCentroid *centroid) {
      SYNCHRONIZED(centroids_) {
        centroids_[id] = centroid;
      }
      return true;
    });
  }

  Future<double> getRelevanceForDoc(string collectionId, ProcessedDocument *doc) {
    return threadPool_.addFuture([this, collectionId, doc](){
      ProcessedCentroid *center;
      SYNCHRONIZED(centroids_) {
        center = centroids_[collectionId];
      }
      return center->score(doc);
    });
  }

  Future<double> getRelevanceForText(string collectionId, string text) {
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

  Future<bool> recompute(string collectionId) {
    return centroidManager_->update(collectionId).then([this, collectionId] (bool updated) -> bool {
      if (updated) {
        return reloadCentroid(collectionId).get();
      }
      return false;
    });
  }
};
