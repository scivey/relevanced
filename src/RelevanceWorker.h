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
  Future<double> getRelevanceForDoc(string collectionId, string docId) {
    return threadPool_.addFuture([this, collectionId, docId](){
      return this->relevanceManager_->getRelevanceForDoc(collectionId, docId);
    });
  }
  Future<double> getRelevanceForText(string collectionId, string text) {
    return threadPool_.addFuture([this, collectionId, text](){
      return this->relevanceManager_->getRelevanceForText(collectionId, text);
    });
  }
  Future<string> createDocument(string text) {
    return threadPool_.addFuture([this, text](){
      return this->relevanceManager_->createDocument(text);
    });
  }
  Future<bool> createDocumentWithId(string id, string text) {
    return threadPool_.addFuture([this, id, text](){
      return this->relevanceManager_->createDocumentWithId(id, text);
    });
  }
  Future<bool> deleteDocument(string id) {
    return threadPool_.addFuture([this, id](){
      return this->relevanceManager_->deleteDocument(id);
    });
  }
  Future<string> getDocument(string docId) {
    return threadPool_.addFuture([this, docId](){
      return this->relevanceManager_->getDocument(docId);
    });
  }
  Future<bool> createCollection(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->createCollection(collectionId);
    });
  }
  Future<bool> deleteCollection(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->deleteCollection(collectionId);
    });
  }
  Future<vector<string>> listCollectionDocuments(string collId) {
    return threadPool_.addFuture([this, collId](){
      return this->relevanceManager_->listCollectionDocuments(collId);
    });
  }
  Future<bool> addPositiveDocumentToCollection(string cId, string artId) {
    return threadPool_.addFuture([this, cId, artId](){
      return this->relevanceManager_->addPositiveDocumentToCollection(cId, artId);
    });
  }
  Future<bool> addNegativeDocumentToCollection(string cId, string artId) {
    return threadPool_.addFuture([this, cId, artId](){
      return this->relevanceManager_->addNegativeDocumentToCollection(cId, artId);
    });
  }
  Future<bool> removeDocumentFromCollection(string cId, string artId) {
    return threadPool_.addFuture([this, cId, artId](){
      return this->relevanceManager_->removeDocumentFromCollection(cId, artId);
    });
  }
  Future<string> addNewPositiveDocumentToCollection(string cId, string text) {
    return threadPool_.addFuture([this, cId, text](){
      return this->relevanceManager_->addNewPositiveDocumentToCollection(cId, text);
    });
  }
  Future<string> addNewNegativeDocumentToCollection(string cId, string text) {
    return threadPool_.addFuture([this, cId, text](){
      return this->relevanceManager_->addNewNegativeDocumentToCollection(cId, text);
    });
  }
  Future<bool> recompute(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->recompute(collectionId);
    });
  }
  Future<vector<string>> listCollections() {
    return threadPool_.addFuture([this](){
      return this->relevanceManager_->listCollections();
    });
  }
  Future<vector<string>> listDocuments() {
    return threadPool_.addFuture([this](){
      return this->relevanceManager_->listDocuments();
    });
  }
  Future<vector<string>> listUnassociatedDocuments() {
    return threadPool_.addFuture([this](){
      return this->relevanceManager_->listUnassociatedDocuments();
    });
  }
  Future<int64_t> getCollectionSize(string collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      return this->relevanceManager_->getCollectionSize(collectionId);
    });
  }
};
