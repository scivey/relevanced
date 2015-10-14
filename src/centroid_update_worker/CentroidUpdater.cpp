#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <glog/logging.h>

#include <folly/Format.h>
#include <folly/futures/Try.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/Optional.h>

#include "centroid_update_worker/CentroidUpdater.h"
#include "models/Centroid.h"
#include "models/ProcessedDocument.h"
#include "models/WordVector.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "util/util.h"
#include "util/Clock.h"

using namespace std;
using namespace folly;

namespace relevanced {
namespace centroid_update_worker {
using models::WordVector;
using models::Centroid;
using models::ProcessedDocument;
using thrift_protocol::ECentroidDoesNotExist;
using util::UniquePointer;

CentroidUpdater::CentroidUpdater(
    shared_ptr<persistence::PersistenceIf> persistence,
    shared_ptr<persistence::CentroidMetadataDbIf> metadataDb,
    shared_ptr<util::ClockIf> clock,
    string centroidId)
    : persistence_(persistence),
      centroidMetadataDb_(metadataDb),
      clock_(clock),
      centroidId_(centroidId) {}

Try<bool> CentroidUpdater::run() {
  LOG(INFO) << "CentroidUpdater::run";
  if (!persistence_->doesCentroidExist(centroidId_).get()) {
    LOG(INFO) << format("centroid '{}' does not exist!", centroidId_);
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }

  const uint64_t startTimestamp = clock_->getEpochTime();
  const size_t idListBatchSize = 500;
  const size_t documentBatchSize = 20;


  LOG(INFO) << "listing iniitial document ids...";
  auto firstIdSet = persistence_->listCentroidDocumentRangeFromOffsetOption(
                                      centroidId_, 0, idListBatchSize).get();
  if (!firstIdSet.hasValue()) {
    LOG(INFO) << format(
        "received falsy document list for centroid '{}'; it must have been "
        "deleted. aborting.",
        centroidId_);
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }

  vector<string> idSet = firstIdSet.value();
  map<string, double> centroidScores;
  size_t docCount{0};
  do {
    LOG(INFO) << format("looping: current id set batch size: {}", idSet.size());

    // start the next set of IDs loading here so they'll be done
    // at the end of the current loop.
    auto nextIdSetFuture =
        persistence_->listCentroidDocumentRangeFromDocumentIdOption(
            centroidId_, idSet.back(), idListBatchSize);

    for (size_t docNum = 0; docNum < idSet.size();
         docNum += documentBatchSize) {
      size_t lastDocIndex = min(idSet.size() - 1, docNum + documentBatchSize);
      vector<Future<Optional<shared_ptr<ProcessedDocument>>>> documentFutures;

      // get batch of documents in parallel
      for (size_t i = docNum; i <= lastDocIndex; i++) {
        documentFutures.push_back(
            persistence_->loadDocumentOption(idSet.at(i)));
      }

      LOG(INFO) << "loading document batch...";
      auto loadedDocuments = collect(documentFutures).get();

      // add non-falsy documents to the centroid
      for (size_t i = 0; i < loadedDocuments.size(); i++) {
        auto doc = loadedDocuments.at(i);
        if (!doc.hasValue()) {
          auto docId = idSet.at(i);
          LOG(INFO) << format("missing document '{}' from centroid '{}'", docId,
                              centroidId_);
          persistence_->removeDocumentFromCentroid(centroidId_, docId);
          continue;
        }
        LOG(INFO) << format("adding document: '{}'", idSet.at(i));
        docCount++;
        for (auto &elem : doc.value()->wordVector.scores) {
          if (centroidScores.find(elem.first) == centroidScores.end()) {
            centroidScores[elem.first] = elem.second;
          } else {
            centroidScores[elem.first] += elem.second;
          }
        }
      }
    }

    auto nextIdSet = nextIdSetFuture.get();
    if (!nextIdSet.hasValue()) {
      LOG(INFO) << format(
          "received falsy document list for centroid '{}'; it must have been "
          "deleted. aborting.",
          centroidId_);
      return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
    }
    idSet = std::move(nextIdSet.value());

  } while (idSet.size() >= idListBatchSize);

  double centroidMagnitude = 0.0;
  for (auto &elem : centroidScores) {
    centroidMagnitude += pow(elem.second, 2);
  }
  centroidMagnitude = sqrt(centroidMagnitude);

  auto centroid = make_shared<Centroid>(centroidId_);
  centroid->wordVector.scores = centroidScores;
  centroid->wordVector.magnitude = centroidMagnitude;
  centroid->wordVector.documentWeight = docCount;
  LOG(INFO) << "persisting...";
  persistence_->saveCentroid(centroidId_, centroid).get();
  centroidMetadataDb_->setLastCalculatedTimestamp(centroidId_, startTimestamp);
  LOG(INFO) << "persisted..";
  return Try<bool>(true);
}

} // centroid_update_worker
} // relevanced