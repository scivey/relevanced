#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <glog/logging.h>
#include <folly/Format.h>
#include <folly/Optional.h>
#include "util.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "CentroidFactory.h"
#include "CentroidUpdateWorker.h"
#include "persistence/PersistenceService.h"
using namespace persistence;
using namespace std;
using namespace folly;
using util::UniquePointer;

CentroidUpdateWorker::CentroidUpdateWorker(
  shared_ptr<persistence::PersistenceServiceIf> persistence,
  string collectionId
): persistence_(persistence), collectionId_(collectionId) {}

bool CentroidUpdateWorker::run() {
  LOG(INFO) << "CentroidUpdateWorker::run";
  auto collectionDb = persistence_->getCollectionDb().lock();
  auto documentDb = persistence_->getDocumentDb().lock();
  auto centroidDb = persistence_->getCentroidDb().lock();

  this_thread::sleep_for(chrono::milliseconds(500));
  if (!collectionDb->doesCollectionExist(collectionId_).get()) {
    LOG(INFO) << "collection does not exist! : " << collectionId_;
    return false;
  }
  vector<shared_ptr<ProcessedDocument>> goodDocs;
  vector<shared_ptr<ProcessedDocument>> allDocs;
  LOG(INFO) << "listing positive docs.";
  for (auto &id: collectionDb->listPositiveCollectionDocs(collectionId_).get()) {
    auto doc = documentDb->loadDocumentShared(id).get();
    if (!doc.hasValue()) {
      LOG(INFO) << "missing document: " << id;
      collectionDb->removeDocFromCollection(collectionId_, id);
    } else {
      allDocs.push_back(doc.value());
      goodDocs.push_back(doc.value());
    }
  }
  LOG(INFO) << "listing negative docs.";
  size_t negCount = 0;
  for (auto &id: collectionDb->listNegativeCollectionDocs(collectionId_).get()) {
    negCount++;
    auto doc = documentDb->loadDocumentShared(id).get();
    if (!doc.hasValue()) {
      LOG(INFO) << "missing document: " << id;
      collectionDb->removeDocFromCollection(collectionId_, id);
    } else {
      allDocs.push_back(doc.value());
    }
  }
  LOG(INFO) << format("doc counts: all {}   - {}   + {}", allDocs.size(), negCount, goodDocs.size());
  LOG(INFO) << "computing...";

  CentroidFactory centroidFactory(allDocs);
  LOG(INFO) << "making centroid...";
  auto centroid = centroidFactory.makeCentroid(goodDocs);
  LOG(INFO) << "processing centroid...";
  auto processed = centroid->toNewProcessedCentroid();
  LOG(INFO) << "persisting...";
  centroidDb->saveCentroid(collectionId_, processed).get();
  LOG(INFO) << "persisted..";
  return true;
}
