#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

#include "CollectionDB.h"
#include "CollectionDBHandle.h"
#include "SqlDb.h"
#include "util.h"
using namespace std;
using namespace folly;
using namespace wangle;
using util::UniquePointer;
namespace persistence {

CollectionDB::CollectionDB(UniquePointer<CollectionDBHandleIf> dbHandle, shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
  : dbHandle_(std::move(dbHandle)), threadPool_(threadPool) {}

Future<bool> CollectionDB::doesCollectionExist(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->doesCollectionExist(collectionId);
  });
}

Future<bool> CollectionDB::createCollection(const string &collectionId) {
  threadPool_->addFuture([this, collectionId](){
    return dbHandle_->createCollection(collectionId);
  });
  return makeFuture(true);
}

Future<bool> CollectionDB::doesCollectionHaveDoc(const string &collectionId, const string &docId) {
  return threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->doesCollectionHaveDoc(collectionId, docId);
  });
}

Future<bool> CollectionDB::addPositiveDocToCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->addPositiveDocToCollection(collectionId, docId);
  });
  return makeFuture(true);
}

Future<bool> CollectionDB::addNegativeDocToCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->addNegativeDocToCollection(collectionId, docId);
  });
  return makeFuture(true);
}

Future<bool> CollectionDB::removeDocFromCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->removeDocFromCollection(collectionId, docId);
  });
  return makeFuture(true);
}

Future<bool> CollectionDB::deleteCollection(const string &collectionId) {
  threadPool_->addFuture([this, collectionId](){
    return dbHandle_->deleteCollection(collectionId);
  });
  return makeFuture(true);
}

Future<vector<string>> CollectionDB::listCollections() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listCollections();
  });
}

Future<int> CollectionDB::getCollectionDocCount(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->getCollectionDocCount(collectionId);
  });
}

Future<vector<string>> CollectionDB::listCollectionDocs(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->listCollectionDocs(collectionId);
  });
}

Future<vector<string>> CollectionDB::listPositiveCollectionDocs(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->listPositiveCollectionDocs(collectionId);
  });
}

Future<vector<string>> CollectionDB::listNegativeCollectionDocs(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->listPositiveCollectionDocs(collectionId);
  });
}

Future<vector<string>> CollectionDB::listKnownDocuments() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listKnownDocuments();
  });
}

} // persistence
