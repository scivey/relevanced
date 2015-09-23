#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>

#include "util.h"
#include "Document.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "CentroidFactory.h"
#include "persistence/CollectionDB.h"
#include "persistence/DocumentDB.h"
#include "persistence/CentroidDB.h"
#include "CentroidManager.h"

namespace {
  using namespace std;
}

class RelevanceCollectionManager {
protected:
  persistence::CollectionDB* collectionDb_;
  persistence::DocumentDB* documentDb_;
  persistence::CentroidDB* centroidDb_;
  CentroidManager* centroidManager_;
  map<string, ProcessedCentroid*> centroids_;
  shared_ptr<DocumentProcessor> documentProcessor_;
public:
  RelevanceCollectionManager(shared_ptr<DocumentProcessor> documentProcessor)
    : documentProcessor_(documentProcessor) {
    centroidManager_ = CentroidManager::getInstance();
    collectionDb_ = persistence::CollectionDB::getInstance();
    documentDb_ = persistence::DocumentDB::getInstance();
    centroidDb_ = persistence::CentroidDB::getInstance();
  }
  void initialize() {
    LOG(INFO) << "[ loading... ]";
    auto collectionIds = collectionDb_->listCollections().get();
    for (auto &id: collectionIds) {
      LOG(INFO) << "[ loading centroid: " << id << " ... ]";
      ProcessedCentroid *centroid = centroidManager_->getCentroid(id).get();
      centroids_[id] = centroid;
    }
    LOG(INFO) << "[ done loading ]";
  }
  double getRelevanceForDoc(const string &collectionId, const string &docId) {
    if (centroids_.find(collectionId) == centroids_.end()) {
      if (!centroidDb_->doesCentroidExist(collectionId).get()) {
        return -999.9;
      }
      auto centroid = centroidManager_->getCentroid(collectionId).get();
      centroids_[collectionId] = centroid;
    }
    if (!documentDb_->doesDocumentExist(docId).get()) {
      return -999.0;
    }
    auto doc = documentDb_->loadDocument(docId).get();
    return centroids_[collectionId]->score(doc);
  }
  double getRelevanceForText(const string &collectionId, const string &text) {
    if (centroids_.find(collectionId) == centroids_.end()) {
      if (!centroidDb_->doesCentroidExist(collectionId).get()) {
        return -999.9;
      }
      auto centroid = centroidManager_->getCentroid(collectionId).get();
      centroids_[collectionId] = centroid;
    }
    Document doc("no-id", text);
    auto processed = documentProcessor_->process(doc);
    return centroids_[collectionId]->score(&processed);
  }
  bool createCollection(string id) {
    if (centroids_.find(id) != centroids_.end()) {
      return false;
    }
    collectionDb_->createCollection(id);
    return true;
  }
  bool deleteCollection(string id) {
    if (centroids_.find(id) != centroids_.end()) {
      return false;
    }
    delete centroids_[id];
    centroids_.erase(id);
    collectionDb_->deleteCollection(id);
    return true;
  }
  bool deleteDocument(string id) {
    return true;
  }
  string createDocument(string text) {
    Document doc(util::getUuid(), text);
    auto processed = documentProcessor_->processNew(doc);
    documentDb_->saveDocument(processed).get();
    return processed->id;
  }
  bool createDocumentWithId(string id, string text) {
    if (documentDb_->doesDocumentExist(id).get()) {
      return false;
    }
    Document doc(id, text);
    auto processed = documentProcessor_->processNew(doc);
    documentDb_->saveDocument(processed).get();
    return true;
  }
  string addNewNegativeDocumentToCollection(string collectionId, string text) {
    return "";
  }
  string addNewPositiveDocumentToCollection(string collectionId, string text) {
    return "";
  }
  bool reloadCentroid(string id) {
    ProcessedCentroid *centroid = centroidManager_->getCentroid(id).get();
    centroids_[id] = centroid;
    return true;
  }
};

