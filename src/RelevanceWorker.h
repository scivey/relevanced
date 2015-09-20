#pragma once

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "RelevanceCollectionManager.h"

namespace {
  using namespace wangle;
  using namespace folly;

}
class RelevanceWorker {
protected:
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {1};
  RelevanceCollectionManager *relevanceManager_;
public:
  RelevanceWorker(RelevanceCollectionManager *manager): relevanceManager_(manager) {
  }
  Future<bool> addPositiveToCollection(string cId, string artId, string text) {
    return threadPool_.addFuture([this, cId, artId, text](){
      return this->relevanceManager_->addPositiveToCollection(cId, artId, text);
    });
  }
  Future<bool> addNegativeToCollection(string cId, string artId, string text) {
    return threadPool_.addFuture([this, cId, artId, text](){
      return this->relevanceManager_->addNegativeToCollection(cId, artId, text);
    });
  }
  Future<bool> createCollection(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->createCollection(collectionId);
    });
  }
  Future<bool> recompute(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->recompute(collectionId);
    });
  }
  Future<double> getRelevance(string collectionId, string text) {
    return threadPool_.addFuture([this, collectionId, text](){
      return this->relevanceManager_->getRelevance(collectionId, text);
    });
  }
  Future<vector<string>> listCollections() {
    return threadPool_.addFuture([this](){
      return this->relevanceManager_->listCollections();
    });
  }
  Future<int64_t> getCollectionSize(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->getCollectionSize(collectionId);
    });
  }
};
