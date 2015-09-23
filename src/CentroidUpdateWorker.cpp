#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>
#include "util.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "CentroidFactory.h"
#include "persistence/PersistenceService.h"

using namespace persistence;
using namespace std;

CentroidUpdateWorker::CentroidUpdateWorker(
  shared_ptr<persistence::PersistenceService> persistence,
  string collectionId
): persistence_(std::move(persistence)), collectionId_(collectionId) {}

bool CentroidUpdateWorker::run() {
  auto collectionDb = persistence_->getCollectionDb().lock();
  auto documentDb = persistence_->getDocumentDb().lock();
  auto centroidDb = persistence_->getCentroidDb().lock();

  if (!collectionDb->doesCollectionExist(collectionId_).get()) {
    return false;
  }
  auto docIds = collectionDb->listCollectionDocs(collectionId_).get();
  vector<ProcessedDocument*> goodDocs;
  vector<ProcessedDocument*> allDocs;
  for (auto &id: collectionDb->listPositiveCollectionDocs(collectionId_).get()) {
    auto doc = documentDb->loadDocument(id).get();
    allDocs.push_back(doc);
    goodDocs.push_back(doc);
  }
  for (auto &id: collectionDb->listNegativeCollectionDocs(collectionId_).get()) {
    auto doc = documentDb->loadDocument(id).get();
    allDocs.push_back(doc);
  }
  CentroidFactory centroidFactory(allDocs);
  auto centroid = centroidFactory.makeCentroid(goodDocs);
  auto processed = centroid->toNewProcessedCentroid();
  centroidDb->saveCentroid(collectionId_, processed);
  return true;
}
