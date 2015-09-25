#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <glog/logging.h>
#include "CollectionDB.h"
#include "CollectionDBHandle.h"
#include "CollectionDBCache.h"
#include "SqlDb.h"
#include "util.h"

using namespace std;
using namespace folly;
using namespace wangle;
using util::UniquePointer;
namespace persistence {

CollectionDB::CollectionDB(UniquePointer<CollectionDBHandleIf> dbHandle, shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
  : dbHandle_(std::move(dbHandle)), threadPool_(threadPool) {
    dbCache_ = std::make_shared<CollectionDBCache>();
  }

void CollectionDB::initialize() {
  // only called on startup -- blocking is ok.
  LOG(INFO) << "initializing CollectionDB cache.";
  vector<string> collectionIds = threadPool_->addFuture([this](){
    return dbHandle_->listCollections();
  }).get();
  for (auto &collId: collectionIds) {
    LOG(INFO) << "initializing cache for " << collId;
    dbCache_->createCollection(collId);
    vector<string> positiveDocs = threadPool_->addFuture([this, collId](){
      return dbHandle_->listPositiveCollectionDocs(collId);
    }).get();
    LOG(INFO) << "positive docs for " << collId << " : " << positiveDocs.size();
    for (auto &docId: positiveDocs) {
      dbCache_->addPositiveDocToCollection(collId, docId);
    }
    vector<string> negativeDocs = threadPool_->addFuture([this, collId](){
      return dbHandle_->listNegativeCollectionDocs(collId);
    }).get();
    LOG(INFO) << "negative docs for " << collId << " : " << negativeDocs.size();
    for (auto &docId: negativeDocs) {
      dbCache_->addNegativeDocToCollection(collId, docId);
    }
  }
  LOG(INFO) << "CollectionDB initialized.";
}

Future<bool> CollectionDB::doesCollectionExist(const string &collectionId) {
  return makeFuture(dbCache_->doesCollectionExist(collectionId));
}

Future<bool> CollectionDB::createCollection(const string &collectionId) {
  threadPool_->addFuture([this, collectionId](){
    return dbHandle_->createCollection(collectionId);
  });
  dbCache_->createCollection(collectionId);
  return makeFuture(true);
}

Future<bool> CollectionDB::doesCollectionHaveDoc(const string &collectionId, const string &docId) {
  return makeFuture(dbCache_->doesCollectionHaveDoc(collectionId, docId));
}

Future<bool> CollectionDB::addPositiveDocToCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->addPositiveDocToCollection(collectionId, docId);
  });
  dbCache_->addPositiveDocToCollection(collectionId, docId);
  return makeFuture(true);
}

Future<bool> CollectionDB::addNegativeDocToCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->addNegativeDocToCollection(collectionId, docId);
  });
  dbCache_->addNegativeDocToCollection(collectionId, docId);
  return makeFuture(true);
}

Future<bool> CollectionDB::removeDocFromCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->removeDocFromCollection(collectionId, docId);
  });
  dbCache_->removeDocFromCollection(collectionId, docId);
  return makeFuture(true);
}

Future<bool> CollectionDB::deleteCollection(const string &collectionId) {
  threadPool_->addFuture([this, collectionId](){
    return dbHandle_->deleteCollection(collectionId);
  });
  dbCache_->deleteCollection(collectionId);
  return makeFuture(true);
}

Future<vector<string>> CollectionDB::listCollections() {
  return makeFuture(dbCache_->listCollections());
}

Future<int> CollectionDB::getCollectionDocCount(const string &collectionId) {
  return makeFuture(dbCache_->getCollectionDocCount(collectionId));
}

Future<vector<string>> CollectionDB::listCollectionDocs(const string &collectionId) {
  return makeFuture(dbCache_->listCollectionDocs(collectionId));
}

Future<vector<string>> CollectionDB::listPositiveCollectionDocs(const string &collectionId) {
  return makeFuture(dbCache_->listPositiveCollectionDocs(collectionId));
}

Future<vector<string>> CollectionDB::listNegativeCollectionDocs(const string &collectionId) {
  return makeFuture(dbCache_->listNegativeCollectionDocs(collectionId));
}

Future<vector<string>> CollectionDB::listKnownDocuments() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listKnownDocuments();
  });
}

} // persistence
