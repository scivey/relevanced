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
#include "util.h"

using namespace std;
using namespace folly;
using namespace wangle;
using util::UniquePointer;
namespace persistence {

CollectionDB::CollectionDB(UniquePointer<CollectionDBHandleIf> dbHandle, shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
  : dbHandle_(std::move(dbHandle)), threadPool_(threadPool) {
  }

void CollectionDB::initialize() {
  LOG(INFO) << "CollectionDB initialized.";
}

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

Future<bool> CollectionDB::doesCollectionHaveDocument(const string &collectionId, const string &docId) {
  return threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->doesCollectionHaveDocument(collectionId, docId);
  });
}

Future<bool> CollectionDB::addPositiveDocumentToCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->addPositiveDocumentToCollection(collectionId, docId);
  });
  return makeFuture(true);
}

Future<bool> CollectionDB::addNegativeDocumentToCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->addNegativeDocumentToCollection(collectionId, docId);
  });
  return makeFuture(true);
}

Future<bool> CollectionDB::removeDocumentFromCollection(const string &collectionId, const string &docId) {
  threadPool_->addFuture([this, collectionId, docId](){
    return dbHandle_->removeDocumentFromCollection(collectionId, docId);
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

Future<int> CollectionDB::getCollectionDocumentCount(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->getCollectionDocumentCount(collectionId);
  });
}

Future<vector<string>> CollectionDB::listCollectionDocuments(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->listCollectionDocuments(collectionId);
  });
}

Future<vector<string>> CollectionDB::listPositiveCollectionDocuments(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->listPositiveCollectionDocuments(collectionId);
  });
}

Future<vector<string>> CollectionDB::listNegativeCollectionDocuments(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    return dbHandle_->listNegativeCollectionDocuments(collectionId);
  });
}

Future<vector<string>> CollectionDB::listKnownDocuments() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listKnownDocuments();
  });
}

} // persistence
