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
#include "Centroid.h"
#include "Vocabulary.h"
#include "VocabularyBuilder.h"
#include "CentroidUpdateWorker.h"
#include "persistence/PersistenceService.h"
using namespace persistence;
using namespace std;
using namespace folly;
using util::UniquePointer;

CentroidUpdateWorker::CentroidUpdateWorker(
  shared_ptr<persistence::PersistenceServiceIf> persistence,
  string classifierId
): persistence_(persistence), classifierId_(classifierId) {}


bool CentroidUpdateWorker::run() {
  LOG(INFO) << "CentroidUpdateWorker::run";
  auto classifierDb = persistence_->getClassifierDb().lock();
  auto documentDb = persistence_->getDocumentDb().lock();
  auto centroidDb = persistence_->getCentroidDb().lock();
  if (!classifierDb->doesClassifierExist(classifierId_).get()) {
    LOG(INFO) << "classifier does not exist! : " << classifierId_;
    return false;
  }
  VocabularyBuilder vocabBuilder;
  set<string> vocabulary;
  LOG(INFO) << "building vocabulary for " << classifierId_;
  for (auto &id: classifierDb->listAllClassifierDocuments(classifierId_).get()) {
    auto doc = documentDb->loadDocument(id).get();
    if (!doc.hasValue()) {
      LOG(INFO) << "missing document: " << id;
      classifierDb->removeDocumentFromClassifier(classifierId_, id);
    } else {
      vocabBuilder.addDocument(doc.value().get());
    }
  }

  auto vocab = vocabBuilder.build();
  Eigen::SparseVector<double> centroidVec(vocab->size());
  size_t documentCount = 0;
  LOG(INFO) << "building centroid for " << classifierId_;
  for (auto &id: classifierDb->listAllPositiveClassifierDocuments(classifierId_).get()) {
    auto doc = documentDb->loadDocument(id).get();
    if (!doc.hasValue()) {
      LOG(INFO) << "missing document: " << id;
      classifierDb->removeDocumentFromClassifier(classifierId_, id);
    } else {
      LOG(INFO) << "adding document scores: " << id;
      auto docVec = vocab->vecOfDocument(doc.value().get());
      centroidVec += docVec;
      documentCount++;
    }
  }
  centroidVec = centroidVec / documentCount;
  auto centroid = make_shared<Centroid>(centroidVec, vocab);
  LOG(INFO) << "persisting...";
  centroidDb->saveCentroid(classifierId_, centroid).get();
  LOG(INFO) << "persisted..";
  return true;
}
