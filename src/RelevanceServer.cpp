#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <folly/Format.h>


#include <glog/logging.h>

#include "gen-cpp2/Relevance.h"
#include "RelevanceScoreWorker.h"

#include "persistence/Persistence.h"
#include "DocumentProcessor.h"
#include "Document.h"
#include "util.h"
#include "RelevanceServer.h"
#include "serialization/serializers.h"
#include "ProcessedDocument.h"

using namespace std;
using namespace folly;
using util::UniquePointer;

RelevanceServer::RelevanceServer(
  shared_ptr<RelevanceScoreWorkerIf> scoreWorker,
  shared_ptr<DocumentProcessingWorkerIf> docProcessor,
  shared_ptr<persistence::PersistenceIf> persistenceSv
): scoreWorker_(scoreWorker), processingWorker_(docProcessor), persistence_(persistenceSv) {}

void RelevanceServer::ping() {}

Future<double> RelevanceServer::getDocumentSimilarity(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  string coll = *centroidId;
  return persistence_->loadDocumentOption(*docId).then([this, coll](Optional<shared_ptr<ProcessedDocument>> doc) {
    if (!doc.hasValue()) {
      return makeFuture(0.0);
    }
    return scoreWorker_->getDocumentSimilarity(coll, doc.value());
  });
}

Future<double> RelevanceServer::getTextSimilarity(unique_ptr<string> centroidId, unique_ptr<string> text) {
  auto doc = std::make_shared<Document>("no-id", *text);
  auto cId = *centroidId;
  return processingWorker_->processNew(doc).then([this, cId](shared_ptr<ProcessedDocument> processed) {
    return scoreWorker_->getDocumentSimilarity(cId, processed);
  });
}

Future<unique_ptr<string>> RelevanceServer::createDocument(unique_ptr<string> text) {
  return internalCreateDocumentWithID(util::getUuid(), *text);
}

Future<unique_ptr<string>> RelevanceServer::internalCreateDocumentWithID(string id, string text) {
  auto doc = std::make_shared<Document>(id, text);
  LOG(INFO) << "creating document: " << id.substr(0, 15) << "  |   " << text.substr(0, 20);
  processingWorker_->processNew(doc).then([this](shared_ptr<ProcessedDocument> processed) {
    persistence_->saveDocument(processed);
  });
  return makeFuture(std::make_unique<string>(id));
}

Future<unique_ptr<string>> RelevanceServer::createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) {
  return internalCreateDocumentWithID(*id, *text);
}

Future<bool> RelevanceServer::deleteDocument(unique_ptr<string> id) {
  persistence_->deleteDocument(*id);
  return makeFuture(true);
}

Future<unique_ptr<string>> RelevanceServer::getDocument(unique_ptr<string> id) {
  return persistence_->loadDocumentOption(*id).then([](Optional<shared_ptr<ProcessedDocument>> proced) {
    if (!proced.hasValue()) {
      auto uniq = std::make_unique<string>("{}");
      return std::move(uniq);
    }
    auto uniq = std::make_unique<string>(serialization::jsonSerialize(proced.value().get()));
    return std::move(uniq);
  });
}

Future<bool> RelevanceServer::createCentroid(unique_ptr<string> centroidId) {
  auto id = *centroidId;
  LOG(INFO) << format("createCentroid: {}", id);
  return persistence_->createNewCentroid(id).then([id](Try<bool> result) {
    return true;
  });
}

Future<bool> RelevanceServer::deleteCentroid(unique_ptr<string> centroidId) {
  persistence_->deleteCentroid(*centroidId);
  return makeFuture(true);
}

Future<unique_ptr<vector<string>>> RelevanceServer::listAllDocumentsForCentroid(unique_ptr<string> centroidId) {
  auto id = *centroidId;
  LOG(INFO) << "listing documents for: " << id;
  return persistence_->listAllDocumentsForCentroid(id).then([id](Try<vector<string>> docIds) {
    if (docIds.hasValue()) {
      return std::move(std::make_unique<vector<string>>(std::move(docIds.value())));
    }
    return std::move(std::make_unique<vector<string>>());
  });
}

Future<bool> RelevanceServer::addDocumentToCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  auto dId = *docId;
  LOG(INFO) << "adding document to " << cId << " : " << dId;
  return persistence_->addDocumentToCentroid(cId, dId).then([this, cId](Try<bool> result) {
    if (result.hasException()) {
      return false;
    }
    scoreWorker_->triggerUpdate(cId);
    return true;
  });
}

Future<bool> RelevanceServer::removeDocumentFromCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  return persistence_->removeDocumentFromCentroid(cId, *docId).then([this, cId](Try<bool> result){
    if (result.hasException()) {
      return false;
    }
    scoreWorker_->triggerUpdate(cId);
    return true;
  });
}

Future<bool> RelevanceServer::recomputeCentroid(unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  LOG(INFO) << "recomputing centroid: " << cId;
  return scoreWorker_->recomputeCentroid(cId);
}

Future<unique_ptr<vector<string>>> RelevanceServer::listAllCentroids() {
  return persistence_->listAllCentroids().then([](vector<string> centroidIds){
    return std::move(std::make_unique<vector<string>>(centroidIds));
  });
}

Future<unique_ptr<vector<string>>> RelevanceServer::listAllDocuments() {
  return persistence_->listAllDocuments().then([](vector<string> docIds) {
    return std::move(std::make_unique<vector<string>>(docIds));
  });
}
