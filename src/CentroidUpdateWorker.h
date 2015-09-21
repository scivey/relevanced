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
#include "persistence/CollectionDB.h"
#include "persistence/DocumentDB.h"
#include "persistence/CentroidDB.h"

class CentroidUpdateWorker {
protected:
  persistence::CollectionDB *collectionDb_;
  persistence::DocumentDB *documentDb_;
  persistence::CentroidDB *centroidDb_;
  string collectionId_;
public:
  CentroidUpdateWorker(string collectionId): collectionId_(collectionId) {
    collectionDb_ = persistence::CollectionDB::getInstance();
    documentDb_ = persistence::DocumentDB::getInstance();
    centroidDb_ = persistence::CentroidDB::getInstance();
  }
  bool run() {
    if (!collectionDb_->doesCollectionExist(collectionId_).get()) {
      return false;
    }
    auto docIds = collectionDb_->listCollectionDocs(collectionId_).get();
    vector<ProcessedDocument*> goodDocs;
    vector<ProcessedDocument*> allDocs;
    for (auto &id: collectionDb_->listPositiveCollectionDocs(collectionId_).get()) {
      auto doc = documentDb_->loadDocument(id).get();
      allDocs.push_back(doc);
      goodDocs.push_back(doc);
    }
    for (auto &id: collectionDb_->listNegativeCollectionDocs(collectionId_).get()) {
      auto doc = documentDb_->loadDocument(id).get();
      allDocs.push_back(doc);
    }
    CentroidFactory centroidFactory(allDocs);
    auto centroid = centroidFactory.makeCentroid(goodDocs);
    auto processed = centroid->toNewProcessedCentroid();
    centroidDb_->saveCentroid(collectionId_, processed);
    return true;
  }
};