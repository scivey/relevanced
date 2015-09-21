#pragma once

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <vector>
#include "RelevanceCollectionManager.h"
#include "persistence/DocumentDB.h"
#include "persistence/CentroidDB.h"
#include "persistence/CollectionDB.h"
#include "CentroidManager.h"

namespace {
  using namespace wangle;
  using namespace folly;
}
class RelevanceWorker {
protected:
  CentroidManager *centroidManager_;
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {1};
  RelevanceCollectionManager *relevanceManager_;
  persistence::DocumentDB *documentDb_;
  persistence::CollectionDB *collectionDb_;
  persistence::CentroidDB *centroidDb_;
public:
  RelevanceWorker(RelevanceCollectionManager *manager): relevanceManager_(manager) {
    documentDb_ = persistence::DocumentDB::getInstance();
    collectionDb_ = persistence::CollectionDB::getInstance();
    centroidDb_ = persistence::CentroidDB::getInstance();
    centroidManager_ = CentroidManager::getInstance();
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
    return documentDb_->doesDocumentExist(docId).then([this, docId] (bool doesExist) -> string {
      if (!doesExist) {
        return "";
      }
      return docId;
    });
  }
  Future<bool> createCollection(string collectionId) {
    return collectionDb_->createCollection(collectionId);
  }
  Future<bool> deleteCollection(string collectionId) {
    return collectionDb_->deleteCollection(collectionId);
  }
  Future<vector<string>> listCollectionDocuments(string collId) {
    return collectionDb_->listCollectionDocs(collId);
  }
  Future<bool> addPositiveDocumentToCollection(string cId, string artId) {
    return documentDb_->doesDocumentExist(artId).then([this, cId, artId] (bool doesExist) -> bool {
      if (!doesExist) {
        LOG(INFO) << "document does not exist: " << artId;
        return false;
      }
      return collectionDb_->addPositiveDocToCollection(cId, artId).get();
    });
  }
  Future<bool> addNegativeDocumentToCollection(string cId, string artId) {
    return documentDb_->doesDocumentExist(artId).then([this, cId, artId] (bool doesExist) -> bool {
      if (!doesExist) {
        LOG(INFO) << "document does not exist: " << artId;
        return false;
      }
      return collectionDb_->addNegativeDocToCollection(cId, artId).get();
    });
  }
  Future<bool> removeDocumentFromCollection(string cId, string artId) {
    return collectionDb_->removeDocFromCollection(cId, artId);
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
    return centroidManager_->update(collectionId).then([this, collectionId] (bool updated) -> bool {
      if (updated) {
        relevanceManager_->reloadCentroid(collectionId);
      }
      return updated;
    });
  }
  Future<vector<string>> listCollections() {
    return collectionDb_->listCollections();
  }
  Future<vector<string>> listDocuments() {
    return collectionDb_->listKnownDocuments();
  }
  Future<vector<string>> listUnassociatedDocuments() {
    return threadPool_.addFuture([this](){
      vector<string> dummy;
      return dummy;
    });
  }
  Future<int> getCollectionSize(string collectionId) {
    return collectionDb_->getCollectionDocCount(collectionId);
  }
};
