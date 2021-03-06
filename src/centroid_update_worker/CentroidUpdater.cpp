#include <chrono>
#include <cmath>
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
#include "centroid_update_worker/DocumentAccumulator.h"

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
    shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory,
    string centroidId)
    : persistence_(persistence),
      centroidMetadataDb_(metadataDb),
      clock_(clock),
      accumulatorFactory_(accumulatorFactory),
      centroidId_(centroidId) {}

Try<bool> CentroidUpdater::run() {
  DLOG(INFO) << "CentroidUpdater: running for " << centroidId_;
  if (!persistence_->doesCentroidExist(centroidId_).get()) {
    LOG(INFO) << format("centroid '{}' does not exist!", centroidId_);
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }

  const uint64_t startTimestamp = clock_->getEpochTime();
  const size_t idListBatchSize = 500;
  const size_t documentBatchSize = 20;


  auto firstIdSet = persistence_->listCentroidDocumentRangeFromOffsetOption(
                                      centroidId_, 0, idListBatchSize).get();
  if (!firstIdSet.hasValue()) {
    LOG(INFO) << format(
        "received falsy document list for centroid '{}'; it must have been "
        "deleted. aborting.",
        centroidId_);
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  auto accumulator = accumulatorFactory_->get();
  vector<string> idSet = std::move(firstIdSet.value());

  if (idSet.size() > 0) {
    do {
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
            util::optionOfTry<shared_ptr<ProcessedDocument>>(
              persistence_->loadDocument(idSet.at(i))
            )
          );
        }

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
          accumulator->addDocument(doc.value().get());
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
  }
  auto centroid = make_shared<Centroid>(centroidId_);
  centroid->wordVector.magnitude = accumulator->getMagnitude();
  centroid->wordVector.documentWeight = accumulator->getCount();
  centroid->wordVector.scores = std::move(accumulator->getScores());
  if (!persistence_->doesCentroidExist(centroidId_).get()) {
    LOG(INFO) << "Centroid missing after update; must have been deleted.";
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  persistence_->saveCentroid(centroidId_, centroid).get();
  centroidMetadataDb_->setLastCalculatedTimestamp(centroidId_, startTimestamp);
  return Try<bool>(true);
}

} // centroid_update_worker
} // relevanced