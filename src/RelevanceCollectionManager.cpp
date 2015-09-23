#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>

#include "RelevanceCollectionManager.h"
#include "util.h"
#include "Document.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "CentroidFactory.h"
#include "persistence/PersistenceService.h"
#include "CentroidManager.h"

using namespace std;
using persistence::PersistenceServiceIf;

RelevanceCollectionManager::RelevanceCollectionManager(
  shared_ptr<PersistenceServiceIf> persistence,
  shared_ptr<CentroidManager> centroidManager,
  shared_ptr<DocumentProcessor> documentProcessor
): persistence_(persistence),
   centroidManager_(centroidManager),
   documentProcessor_(documentProcessor) {}

void RelevanceCollectionManager::initialize() {
  LOG(INFO) << "[ loading... ]";
  auto collectionDb = persistence_->getCollectionDb().lock();
  auto collectionIds = collectionDb->listCollections().get();
  for (auto &id: collectionIds) {
    LOG(INFO) << "[ loading centroid: " << id << " ... ]";
    ProcessedCentroid *centroid = centroidManager_->getCentroid(id).get();
    centroids_[id] = centroid;
  }
  LOG(INFO) << "[ done loading ]";
}

double RelevanceCollectionManager::getRelevanceForDoc(const string &collectionId, const string &docId) {
  if (centroids_.find(collectionId) == centroids_.end()) {
    auto centroidDb = persistence_->getCentroidDb().lock();
    if (!centroidDb->doesCentroidExist(collectionId).get()) {
      return -999.9;
    }
    auto centroid = centroidManager_->getCentroid(collectionId).get();
    centroids_[collectionId] = centroid;
  }
  auto docDb = persistence_->getDocumentDb().lock();
  if (!docDb->doesDocumentExist(docId).get()) {
    return -999.0;
  }
  auto doc = docDb->loadDocument(docId).get();
  return centroids_[collectionId]->score(doc);
}

double RelevanceCollectionManager::getRelevanceForText(const string &collectionId, const string &text) {
  if (centroids_.find(collectionId) == centroids_.end()) {
    auto centroidDb = persistence_->getCentroidDb().lock();
    if (!centroidDb->doesCentroidExist(collectionId).get()) {
      return -999.9;
    }
    auto centroid = centroidManager_->getCentroid(collectionId).get();
    centroids_[collectionId] = centroid;
  }
  Document doc("no-id", text);
  auto processed = documentProcessor_->process(doc);
  return centroids_[collectionId]->score(&processed);
}

bool RelevanceCollectionManager::createCollection(string id) {
  if (centroids_.find(id) != centroids_.end()) {
    return false;
  }
  auto collectionDb = persistence_->getCollectionDb().lock();
  collectionDb->createCollection(id);
  return true;
}

bool RelevanceCollectionManager::deleteCollection(string id) {
  if (centroids_.find(id) != centroids_.end()) {
    return false;
  }
  delete centroids_[id];
  centroids_.erase(id);
  auto collectionDb = persistence_->getCollectionDb().lock();
  collectionDb->deleteCollection(id);
  return true;
}

bool RelevanceCollectionManager::deleteDocument(string id) {
  return true;
}

string RelevanceCollectionManager::createDocument(string text) {
  Document doc(util::getUuid(), text);
  auto processed = documentProcessor_->processNew(doc);
  auto documentDb = persistence_->getDocumentDb().lock();
  documentDb->saveDocument(processed).get();
  return processed->id;
}

bool RelevanceCollectionManager::createDocumentWithId(string id, string text) {
  auto docDb = persistence_->getDocumentDb().lock();
  if (docDb->doesDocumentExist(id).get()) {
    return false;
  }
  Document doc(id, text);
  auto processed = documentProcessor_->processNew(doc);
  docDb->saveDocument(processed).get();
  return true;
}

string RelevanceCollectionManager::addNewNegativeDocumentToCollection(string collectionId, string text) {
  return "";
}

string RelevanceCollectionManager::addNewPositiveDocumentToCollection(string collectionId, string text) {
  return "";
}

bool RelevanceCollectionManager::reloadCentroid(string id) {
  ProcessedCentroid *centroid = centroidManager_->getCentroid(id).get();
  centroids_[id] = centroid;
  return true;
}

