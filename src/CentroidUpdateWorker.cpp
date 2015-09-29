#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <glog/logging.h>
#include <folly/Format.h>
#include <folly/Optional.h>
#include "util.h"
#include "ProcessedDocument.h"
#include "Centroid.h"
#include "CentroidUpdateWorker.h"
#include "persistence/Persistence.h"

using namespace persistence;
using namespace std;
using namespace folly;
using util::UniquePointer;

CentroidUpdateWorker::CentroidUpdateWorker(
  shared_ptr<persistence::PersistenceIf> persistence,
  string centroidId
): persistence_(persistence), centroidId_(centroidId) {}


bool CentroidUpdateWorker::run() {
  LOG(INFO) << "CentroidUpdateWorker::run";
  if (!persistence_->doesCentroidExist(centroidId_).get()) {
    LOG(INFO) << format("centroid '{}' does not exist!", centroidId_);
    return false;
  }

  auto centroidIdsOpt = persistence_->listAllDocumentsForCentroidOption(centroidId_).get();
  if (!centroidIdsOpt.hasValue()) {
    LOG(INFO) << format("falsy document list for centroid '{}'; aborting.", centroidId_);
    return false;
  }

  auto centroidIds = centroidIdsOpt.value();
  map<string, double> centroidScores;
  for (auto &id: centroidIds) {
    auto doc = persistence_->loadDocumentOption(id).get();
    if (!doc.hasValue()) {
      LOG(INFO) << "missing document: " << id;
      persistence_->removeDocumentFromCentroid(centroidId_, id);
    } else {
      auto docPtr = doc.value();
      for (auto &elem: docPtr->normalizedWordCounts) {
        if (centroidScores.find(elem.first) == centroidScores.end()) {
          centroidScores[elem.first] = elem.second;
        } else {
          centroidScores[elem.first] += elem.second;
        }
      }
    }
  }
  double centroidMagnitude = 0.0;
  for (auto &elem: centroidScores) {
    centroidMagnitude += pow(elem.second, 2);
  }
  centroidMagnitude = sqrt(centroidMagnitude);

  auto centroid = make_shared<Centroid>(centroidId_, centroidScores, centroidMagnitude);
  LOG(INFO) << "persisting...";
  persistence_->saveCentroid(centroidId_, centroid).get();
  LOG(INFO) << "persisted..";
  return true;
}
