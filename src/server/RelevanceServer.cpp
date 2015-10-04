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
#include "models/Document.h"
#include "models/ProcessedDocument.h"
#include "persistence/exceptions.h"
#include "persistence/Persistence.h"
#include "serialization/serializers.h"
#include "server/RelevanceServer.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "util/util.h"

namespace relevanced {
namespace server {

using namespace std;
using namespace folly;
using persistence::exceptions::DocumentAlreadyExists;
using persistence::exceptions::CentroidDoesNotExist;

using similarity_score_worker::SimilarityScoreWorkerIf;
using centroid_update_worker::CentroidUpdateWorkerIf;
using document_processing_worker::DocumentProcessingWorkerIf;
using models::Document;
using models::ProcessedDocument;


using util::UniquePointer;

RelevanceServer::RelevanceServer(
  shared_ptr<persistence::PersistenceIf> persistenceSv,
  shared_ptr<SimilarityScoreWorkerIf> scoreWorker,
  shared_ptr<DocumentProcessingWorkerIf> docProcessor,
  shared_ptr<CentroidUpdateWorkerIf> centroidUpdater
): persistence_(persistenceSv), scoreWorker_(scoreWorker),
   processingWorker_(docProcessor), centroidUpdateWorker_(centroidUpdater) {}

void RelevanceServer::ping() {}

void RelevanceServer::initialize() {
  centroidUpdateWorker_->initialize();
  scoreWorker_->initialize();
  centroidUpdateWorker_->onUpdate([this](const string &id) {
    scoreWorker_->reloadCentroid(id);
  });
}

Future<Try<double>> RelevanceServer::getDocumentSimilarity(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  string cId = *centroidId;
  return persistence_->loadDocument(*docId).then([this, cId](Try<shared_ptr<ProcessedDocument>> doc) {
    if (doc.hasException()) {
      return makeFuture<Try<double>>(Try<double>(doc.exception()));
    }
    return scoreWorker_->getDocumentSimilarity(cId, doc.value());
  });
}

Future<Try<double>> RelevanceServer::getCentroidSimilarity(unique_ptr<string> centroid1Id, unique_ptr<string> centroid2Id) {
  string c1 = *centroid1Id;
  string c2 = *centroid2Id;
  return scoreWorker_->getCentroidSimilarity(c1, c2);
}

Future<Try<unique_ptr<map<string, double>>>> RelevanceServer::multiGetTextSimilarity(unique_ptr<vector<string>> centroidIds, unique_ptr<string> text) {
  auto doc = std::make_shared<Document>("no-id", *text);
  shared_ptr<vector<string>> cIds(
    new vector<string>(*centroidIds)
  );
  return processingWorker_->processNew(doc).then([this, cIds](shared_ptr<ProcessedDocument> processed) {
    vector<Future<Try<double>>> scores;
    for (size_t i = 0; i < cIds->size(); i++) {
      scores.push_back(scoreWorker_->getDocumentSimilarity(cIds->at(i), processed));
    }
    return collect(scores).then([this, cIds](Try<vector<Try<double>>> scores) {
      if (scores.hasException()) {
        return Try<unique_ptr<map<string, double>>>(scores.exception());
      }
      auto scoreVals = scores.value();
      auto response = std::make_unique<map<string, double>>();
      for (size_t i = 0; i < cIds->size(); i++) {
        auto currentScoreVal = scoreVals.at(i);
        if (currentScoreVal.hasException()) {
          return Try<unique_ptr<map<string, double>>>(currentScoreVal.exception());
        }
        response->insert(make_pair(cIds->at(i), currentScoreVal.value()));
      }
      return Try<unique_ptr<map<string, double>>>(std::move(response));
    });
  });
}

Future<Try<double>> RelevanceServer::getTextSimilarity(unique_ptr<string> centroidId, unique_ptr<string> text) {
  auto doc = std::make_shared<Document>("no-id", *text);
  auto cId = *centroidId;
  return processingWorker_->processNew(doc).then([this, cId](shared_ptr<ProcessedDocument> processed) {
    return scoreWorker_->getDocumentSimilarity(cId, processed);
  });
}

Future<Try<unique_ptr<string>>> RelevanceServer::createDocument(unique_ptr<string> text) {
  return internalCreateDocumentWithID(util::getUuid(), *text);
}

Future<Try<unique_ptr<string>>> RelevanceServer::internalCreateDocumentWithID(string id, string text) {
  auto doc = std::make_shared<Document>(id, text);
  LOG(INFO) << "creating document: " << id.substr(0, 15) << "  |   " << text.substr(0, 20);
  return processingWorker_->processNew(doc).then([this, id](shared_ptr<ProcessedDocument> processed) {
    return persistence_->saveDocument(processed).then([this, id](Try<bool> result) {
      if (result.hasException()) {
        return Try<unique_ptr<string>>(make_exception_wrapper<DocumentAlreadyExists>());
      }
      return Try<unique_ptr<string>>(std::make_unique<string>(id));
    });
  });
}

Future<Try<unique_ptr<string>>> RelevanceServer::createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) {
  return internalCreateDocumentWithID(*id, *text);
}

Future<Try<bool>> RelevanceServer::deleteDocument(unique_ptr<string> id) {
  return persistence_->deleteDocument(*id);
}

Future<Try<unique_ptr<string>>> RelevanceServer::getDocument(unique_ptr<string> id) {
  return persistence_->loadDocument(*id).then([](Try<shared_ptr<ProcessedDocument>> proced) {
    if (proced.hasException()) {
      return Try<unique_ptr<string>>(proced.exception());
    }
    auto uniq = std::make_unique<string>(serialization::jsonSerialize(proced.value().get()));
    return Try<unique_ptr<string>>(std::move(uniq));
  });
}

Future<Try<bool>> RelevanceServer::createCentroid(unique_ptr<string> centroidId) {
  auto id = *centroidId;
  LOG(INFO) << format("creating centroid '{}'", id);
  return persistence_->createNewCentroid(id);
}

Future<Try<bool>> RelevanceServer::deleteCentroid(unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  LOG(INFO) << format("deleting centroid '{}'", cId);
  return persistence_->deleteCentroid(cId);
}

Future<Try<unique_ptr<vector<string>>>> RelevanceServer::listAllDocumentsForCentroid(unique_ptr<string> centroidId) {
  auto id = *centroidId;
  LOG(INFO) << "listing documents for: " << id;
  return persistence_->listAllDocumentsForCentroid(id).then([id](Try<vector<string>> docIds) {
    if (docIds.hasException()) {
      return Try<unique_ptr<vector<string>>>(docIds.exception());
    }
    vector<string> docs = docIds.value();
    return Try<unique_ptr<vector<string>>>(std::make_unique<vector<string>>(std::move(docs)));
  });
}

Future<Try<bool>> RelevanceServer::addDocumentToCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  auto dId = *docId;
  LOG(INFO) << format("adding document '{}' to centroid '{}'", dId, cId);
  return persistence_->addDocumentToCentroid(cId, dId).then([this, cId](Try<bool> result) {
    if (!result.hasException()) {
      centroidUpdateWorker_->triggerUpdate(cId);
    }
    return result;
  });
}

Future<Try<bool>> RelevanceServer::removeDocumentFromCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  return persistence_->removeDocumentFromCentroid(cId, *docId).then([this, cId](Try<bool> result){
    if (!result.hasException()) {
      centroidUpdateWorker_->triggerUpdate(cId);
    }
    return result;
  });
}

Future<Try<bool>> RelevanceServer::recomputeCentroid(unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  LOG(INFO) << "recomputing centroid: " << cId;
  auto promise = make_shared<Promise<Try<bool>>>();
  centroidUpdateWorker_->onUpdateSpecificOnce(cId, [promise](Try<string> result) {
    if (result.hasException()) {
      promise->setValue(Try<bool>(make_exception_wrapper<CentroidDoesNotExist>()));
    } else {
      promise->setValue(Try<bool>(true));
    }
  });
  centroidUpdateWorker_->triggerUpdate(cId);
  return promise->getFuture();
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

} // server
} // relevanced