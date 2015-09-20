#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "CollectionDBHandle.h"
#include "SqlDb.h"

namespace {
  using namespace std;
  using namespace folly;
  using namespace wangle;
}

namespace persistence {

class CollectionDB {
protected:
  CollectionDBHandle *dbHandle_ {nullptr};
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {1};
  CollectionDB(){
    threadPool_.addFuture([this](){
      auto sqlDb = std::make_unique<SqlDb>("data/collections.sqlite");
      dbHandle_ = new CollectionDBHandle(std::move(sqlDb));
      dbHandle_->ensureTables();
    });
  }
  CollectionDB(CollectionDB const&) = delete;
  void operator=(CollectionDB const&) = delete;
public:
  static CollectionDB* getInstance() {
    static CollectionDB instance;
    return &instance;
  }
  Future<bool> doesCollectionExist(const string &collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return dbHandle_->doesCollectionExist(collectionId);
    });
  }
  Future<bool> createCollection(const string &collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return dbHandle_->createCollection(collectionId);
    });
  }
  Future<bool> doesCollectionHaveDoc(const string &collectionId, const string &docId) {
    return threadPool_.addFuture([this, collectionId, docId](){
      return dbHandle_->doesCollectionHaveDoc(collectionId, docId);
    });
  }
  Future<bool> addDocToCollection(const string &collectionId, const string &docId) {
    return threadPool_.addFuture([this, collectionId, docId](){
      return dbHandle_->addDocToCollection(collectionId, docId);
    });
  }
  Future<bool> addPositiveDocToCollection(const string &collectionId, const string &docId) {
    return threadPool_.addFuture([this, collectionId, docId](){
      return dbHandle_->addPositiveDocToCollection(collectionId, docId);
    });
  }
  Future<bool> addNegativeDocToCollection(const string &collectionId, const string &docId) {
    return threadPool_.addFuture([this, collectionId, docId](){
      return dbHandle_->addNegativeDocToCollection(collectionId, docId);
    });
  }
  Future<bool> removeDocFromCollection(const string &collectionId, const string &docId) {
    return threadPool_.addFuture([this, collectionId, docId](){
      return dbHandle_->removeDocFromCollection(collectionId, docId);
    });
  }
  Future<bool> deleteCollection(const string &collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return dbHandle_->deleteCollection(collectionId);
    });
  }
  Future<vector<string>> listCollectionDocs(const string &collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return dbHandle_->listCollectionDocs(collectionId);
    });
  }
};

} // persistence
