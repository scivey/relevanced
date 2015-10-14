#include <string>
#include <memory>
#include <folly/futures/Promise.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <folly/Format.h>
#include <folly/ExceptionWrapper.h>

#include <glog/logging.h>

#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "models/Document.h"
#include "models/Centroid.h"
#include "models/ProcessedDocument.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "release_metadata/release_metadata.h"
#include "serialization/serializers.h"
#include "server/RelevanceServer.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "util/util.h"
#include "util/Clock.h"

namespace relevanced {
namespace server {

using namespace std;
using namespace folly;
using thrift_protocol::EDocumentAlreadyExists;
using thrift_protocol::ECentroidDoesNotExist;

using similarity_score_worker::SimilarityScoreWorkerIf;
using centroid_update_worker::CentroidUpdateWorkerIf;
using document_processing_worker::DocumentProcessingWorkerIf;
using models::Document;
using models::ProcessedDocument;
using models::Centroid;

using util::UniquePointer;

RelevanceServer::RelevanceServer(
    shared_ptr<persistence::PersistenceIf> persistenceSv,
    shared_ptr<persistence::CentroidMetadataDbIf> metadataDb,
    shared_ptr<util::ClockIf> clock,
    shared_ptr<SimilarityScoreWorkerIf> scoreWorker,
    shared_ptr<DocumentProcessingWorkerIf> docProcessor,
    shared_ptr<CentroidUpdateWorkerIf> centroidUpdater)
    : persistence_(persistenceSv),
      centroidMetadataDb_(metadataDb),
      clock_(clock),
      scoreWorker_(scoreWorker),
      processingWorker_(docProcessor),
      centroidUpdateWorker_(centroidUpdater) {}

void RelevanceServer::ping() {}

void RelevanceServer::initialize() {
  centroidUpdateWorker_->initialize();
  scoreWorker_->initialize();
  centroidUpdateWorker_->onUpdate(
      [this](const string &id) { scoreWorker_->reloadCentroid(id); });
}

Future<Try<double>> RelevanceServer::getDocumentSimilarity(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  string cId = *centroidId;
  return persistence_->loadDocument(*docId)
      .then([this, cId](Try<shared_ptr<ProcessedDocument>> doc) {
        if (doc.hasException()) {
          return makeFuture<Try<double>>(Try<double>(doc.exception()));
        }
        return scoreWorker_->getDocumentSimilarity(cId, doc.value());
      });
}

Future<Try<double>> RelevanceServer::getCentroidSimilarity(
    unique_ptr<string> centroid1Id, unique_ptr<string> centroid2Id) {
  string c1 = *centroid1Id;
  string c2 = *centroid2Id;
  return scoreWorker_->getCentroidSimilarity(c1, c2);
}

Future<Try<unique_ptr<map<string, double>>>>
RelevanceServer::multiGetTextSimilarity(unique_ptr<vector<string>> centroidIds,
                                        unique_ptr<string> text) {
  auto doc = std::make_shared<Document>("no-id", *text);
  shared_ptr<vector<string>> cIds(new vector<string>(*centroidIds));
  return processingWorker_->processNew(doc).then([this, cIds](
      shared_ptr<ProcessedDocument> processed) {
    vector<Future<Try<double>>> scores;
    for (size_t i = 0; i < cIds->size(); i++) {
      scores.push_back(
          scoreWorker_->getDocumentSimilarity(cIds->at(i), processed));
    }
    return collect(scores).then([this, cIds](Try<vector<Try<double>>> scores) {
      if (scores.hasException()) {
        return Try<unique_ptr<map<string, double>>>(scores.exception());
      }
      auto scoreVals = scores.value();
      auto response = folly::make_unique<map<string, double>>();
      for (size_t i = 0; i < cIds->size(); i++) {
        auto currentScoreVal = scoreVals.at(i);
        if (currentScoreVal.hasException()) {
          return Try<unique_ptr<map<string, double>>>(
              currentScoreVal.exception());
        }
        response->insert(make_pair(cIds->at(i), currentScoreVal.value()));
      }
      return Try<unique_ptr<map<string, double>>>(std::move(response));
    });
  });
}

Future<Try<double>> RelevanceServer::getTextSimilarity(
    unique_ptr<string> centroidId, unique_ptr<string> text) {
  auto doc = std::make_shared<Document>("no-id", *text);
  auto cId = *centroidId;
  return processingWorker_->processNew(doc)
      .then([this, cId](shared_ptr<ProcessedDocument> processed) {
        return scoreWorker_->getDocumentSimilarity(cId, processed);
      });
}

Future<Try<unique_ptr<string>>> RelevanceServer::createDocument(
    unique_ptr<string> text) {
  return internalCreateDocumentWithID(util::getUuid(), *text);
}

Future<Try<unique_ptr<string>>> RelevanceServer::internalCreateDocumentWithID(
    string id, string text) {
  auto doc = std::make_shared<Document>(id, text);
  LOG(INFO) << "creating document: " << id.substr(0, 15) << "  |   "
            << text.substr(0, 20);
  return processingWorker_->processNew(doc)
      .then([this, id](shared_ptr<ProcessedDocument> processed) {
        return persistence_->saveDocument(processed)
            .then([this, id](Try<bool> result) {
              if (result.hasException()) {
                return Try<unique_ptr<string>>(
                    make_exception_wrapper<EDocumentAlreadyExists>());
              }
              return Try<unique_ptr<string>>(folly::make_unique<string>(id));
            });
      });
}

Future<Try<unique_ptr<string>>> RelevanceServer::createDocumentWithID(
    unique_ptr<string> id, unique_ptr<string> text) {
  return internalCreateDocumentWithID(*id, *text);
}

Future<Try<bool>> RelevanceServer::deleteDocument(unique_ptr<string> id) {
  return persistence_->deleteDocument(*id);
}

Future<Try<unique_ptr<string>>> RelevanceServer::getDocument(
    unique_ptr<string> id) {
  return persistence_->loadDocument(*id)
      .then([](Try<shared_ptr<ProcessedDocument>> proced) {
        if (proced.hasException()) {
          return Try<unique_ptr<string>>(proced.exception());
        }
        string serialized;
        serialization::jsonSerialize(serialized, proced.value().get());
        auto uniq = folly::make_unique<string>(serialized);
        return Try<unique_ptr<string>>(std::move(uniq));
      });
}

Future<Try<bool>> RelevanceServer::createCentroid(
    unique_ptr<string> centroidId) {
  auto id = *centroidId;
  LOG(INFO) << format("creating centroid '{}'", id);
  return persistence_->createNewCentroid(id);
}

Future<Try<bool>> RelevanceServer::deleteCentroid(
    unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  LOG(INFO) << format("deleting centroid '{}'", cId);
  return persistence_->deleteCentroid(cId);
}

Future<Try<unique_ptr<vector<string>>>>
RelevanceServer::listAllDocumentsForCentroid(unique_ptr<string> centroidId) {
  auto id = *centroidId;
  LOG(INFO) << "listing documents for: " << id;
  return persistence_->listAllDocumentsForCentroid(id)
      .then([id](Try<vector<string>> docIds) {
        if (docIds.hasException()) {
          return Try<unique_ptr<vector<string>>>(docIds.exception());
        }
        vector<string> docs = docIds.value();
        return Try<unique_ptr<vector<string>>>(
            folly::make_unique<vector<string>>(std::move(docs)));
      });
}

Future<Try<bool>> RelevanceServer::addDocumentToCentroid(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  auto dId = *docId;
  LOG(INFO) << format("adding document '{}' to centroid '{}'", dId, cId);
  return persistence_->addDocumentToCentroid(cId, dId)
      .then([this, cId](Try<bool> result) {
        if (!result.hasException()) {
          centroidMetadataDb_->setLastDocumentChangeTimestamp(
              cId, clock_->getEpochTime());
          centroidUpdateWorker_->triggerUpdate(cId);
        }
        return result;
      });
}

Future<Try<bool>> RelevanceServer::removeDocumentFromCentroid(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  return persistence_->removeDocumentFromCentroid(cId, *docId)
      .then([this, cId](Try<bool> result) {
        if (!result.hasException()) {
          centroidMetadataDb_->setLastDocumentChangeTimestamp(
              cId, clock_->getEpochTime());
          centroidUpdateWorker_->triggerUpdate(cId);
        }
        return result;
      });
}

Future<Try<bool>> RelevanceServer::joinCentroid(unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  LOG(INFO) << "recomputing centroid: " << cId;
  return centroidMetadataDb_->isCentroidUpToDate(
                                  cId).then([this, cId](Try<bool> isUpToDate) {
    if (isUpToDate.hasException()) {
      Try<bool> result(isUpToDate.exception());
      return makeFuture(result);
    }
    if (isUpToDate.value()) {
      Try<bool> result(true);
      return makeFuture(result);
    }
    return centroidUpdateWorker_->joinUpdate(cId).then([this, cId](Try<string> result) {
      if (result.hasException()) {
        Try<bool> toReturn(result.exception());
        return makeFuture(toReturn);
      }
      return scoreWorker_->reloadCentroid(cId).then([](){
        return Try<bool>(true);
      });
    });
  });
}

Future<unique_ptr<vector<string>>> RelevanceServer::listAllCentroids() {
  return persistence_->listAllCentroids().then([](vector<string> centroidIds) {
    return std::move(folly::make_unique<vector<string>>(centroidIds));
  });
}

Future<unique_ptr<vector<string>>> RelevanceServer::listAllDocuments() {
  return persistence_->listAllDocuments().then([](vector<string> docIds) {
    return std::move(folly::make_unique<vector<string>>(docIds));
  });
}

Future<unique_ptr<map<string, string>>> RelevanceServer::getServerMetadata() {
  string revision = release_metadata::getGitRevisionSha();
  string version = release_metadata::getGitVersion();
  auto metadata = folly::make_unique<map<string, string>>();
  metadata->insert(make_pair("relevanced_git_revision", revision));
  metadata->insert(make_pair("relevanced_version", version));
  metadata->insert(make_pair("relevanced_utc_build_timestamp",
                             release_metadata::getUtcBuildTimestamp()));
  return makeFuture(std::move(metadata));
}

Future<folly::Unit> RelevanceServer::debugEraseAllData() {
  return persistence_->debugEraseAllData();
}

Future<Try<shared_ptr<Centroid>>> RelevanceServer::debugGetFullCentroid(unique_ptr<string> centroidId) {
  return persistence_->loadCentroid(*centroidId);
}

Future<Try<shared_ptr<ProcessedDocument>>> RelevanceServer::debugGetFullProcessedDocument(unique_ptr<string> documentId) {
  return persistence_->loadDocument(*documentId);
}



} // server
} // relevanced