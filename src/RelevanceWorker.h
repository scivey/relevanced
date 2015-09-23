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
  using namespace std;
}
class RelevanceWorker {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  std::shared_ptr<CentroidManager> centroidManager_;
  RelevanceCollectionManager *relevanceManager_;
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {1};
public:
  RelevanceWorker(
    shared_ptr<persistence::PersistenceServiceIf> persistence,
    shared_ptr<CentroidManager> centroidManager,
    RelevanceCollectionManager *manager
  ): persistence_(persistence),
     centroidManager_(centroidManager),
     relevanceManager_(manager){}

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
    auto docDb = persistence_->getDocumentDb().lock();
    return docDb->doesDocumentExist(docId).then([this, docId] (bool doesExist) -> string {
      if (!doesExist) {
        return "";
      }
      return docId;
    });
  }
  Future<bool> createCollection(string collectionId) {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->createCollection(collectionId);
  }
  Future<bool> deleteCollection(string collectionId) {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->deleteCollection(collectionId);
  }
  Future<vector<string>> listCollectionDocuments(string collId) {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->listCollectionDocs(collId);
  }
  Future<bool> addPositiveDocumentToCollection(string cId, string artId) {
    auto docDb = persistence_->getDocumentDb().lock();
    return docDb->doesDocumentExist(artId).then([this, cId, artId] (bool doesExist) -> bool {
      if (!doesExist) {
        LOG(INFO) << "document does not exist: " << artId;
        return false;
      }
      auto collDb = persistence_->getCollectionDb().lock();
      return collDb->addPositiveDocToCollection(cId, artId).get();
    });
  }
  Future<bool> addNegativeDocumentToCollection(string cId, string artId) {
    auto docDb = persistence_->getDocumentDb().lock();
    return docDb->doesDocumentExist(artId).then([this, cId, artId] (bool doesExist) -> bool {
      if (!doesExist) {
        LOG(INFO) << "document does not exist: " << artId;
        return false;
      }
      auto collDb = persistence_->getCollectionDb().lock();
      return collDb->addNegativeDocToCollection(cId, artId).get();
    });
  }
  Future<bool> removeDocumentFromCollection(string cId, string artId) {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->removeDocFromCollection(cId, artId);
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
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->listCollections();
  }
  Future<vector<string>> listDocuments() {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->listKnownDocuments();
  }
  Future<vector<string>> listUnassociatedDocuments() {
    return threadPool_.addFuture([this](){
      vector<string> dummy;
      return dummy;
    });
  }
  Future<int> getCollectionSize(string collectionId) {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->getCollectionDocCount(collectionId);
  }
};
