#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "RelevanceScoreWorker.h"
#include <folly/Optional.h>
#include "persistence/PersistenceService.h"
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
  shared_ptr<persistence::PersistenceServiceIf> persistenceSv
): scoreWorker_(scoreWorker), processingWorker_(docProcessor), persistence_(persistenceSv) {}

void RelevanceServer::ping() {}

Future<double> RelevanceServer::getRelevanceForDoc(unique_ptr<string> collId, unique_ptr<string> docId) {
  string coll = *collId;
  return persistence_->getDocumentDb().lock()->loadDocumentShared(*docId).then([this, coll](Optional<shared_ptr<ProcessedDocument>> doc) {
    if (!doc.hasValue()) {
      return makeFuture(0.0);
    }
    return scoreWorker_->getRelevanceForDoc(coll, doc.value());
  });
}

Future<double> RelevanceServer::getRelevanceForText(unique_ptr<string> collId, unique_ptr<string> text) {
  auto doc = std::make_shared<Document>("no-id", *text);
  auto classifierId = *collId;
  return processingWorker_->processNew(doc).then([this, classifierId](shared_ptr<ProcessedDocument> processed) {
    return scoreWorker_->getRelevanceForDoc(classifierId, processed);
  });
}

Future<unique_ptr<string>> RelevanceServer::createDocument(unique_ptr<string> text) {
  return internalCreateDocumentWithID(util::getUuid(), *text);
}

Future<unique_ptr<string>> RelevanceServer::internalCreateDocumentWithID(string id, string text) {
  auto doc = std::make_shared<Document>(id, text);
  LOG(INFO) << "creating document: " << id.substr(0, 15) << "  |   " << text.substr(0, 20);
  processingWorker_->processNew(doc).then([this](shared_ptr<ProcessedDocument> processed) {
    persistence_->getDocumentDb().lock()->saveDocument(processed);
  });
  return makeFuture(std::make_unique<string>(id));
}

Future<unique_ptr<string>> RelevanceServer::createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) {
  return internalCreateDocumentWithID(*id, *text);
}

Future<bool> RelevanceServer::deleteDocument(unique_ptr<string> id) {
  return persistence_->getDocumentDb().lock()->deleteDocument(*id);
}

Future<unique_ptr<string>> RelevanceServer::getDocument(unique_ptr<string> id) {
  auto docDb = persistence_->getDocumentDb().lock();
  return docDb->loadDocument(*id).then([](Optional<UniquePointer<ProcessedDocument>> proced) {
    if (!proced.hasValue()) {
      auto uniq = std::make_unique<string>("{}");
      return std::move(uniq);
    }
    auto uniq = std::make_unique<string>(serialization::jsonSerialize(proced.value().get()));
    return std::move(uniq);
  });
}

Future<bool> RelevanceServer::createClassifier(unique_ptr<string> collId) {
  auto id = *collId;
  LOG(INFO) << "creating classifier: " << id;
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->createClassifier(id);
}

Future<bool> RelevanceServer::deleteClassifier(unique_ptr<string> collId) {
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->deleteClassifier(*collId);
}

Future<unique_ptr<vector<string>>> RelevanceServer::listAllClassifierDocuments(unique_ptr<string> collId) {
  auto id = *collId;
  LOG(INFO) << "listing documents for: " << id;
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->listAllClassifierDocuments(id).then([id](vector<string> docIds) {
    LOG(INFO) << "listClassifierDocuments: returning " << docIds.size() << " for " << id;
    return std::move(std::make_unique<vector<string>>(docIds));
  });
}

Future<bool> RelevanceServer::addPositiveDocumentToClassifier(unique_ptr<string> collId, unique_ptr<string> docId) {
  auto coll = *collId;
  auto doc = *docId;
  LOG(INFO) << "adding positive document to " << coll << " : " << doc;
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->addPositiveDocumentToClassifier(coll, doc).then([this, coll](bool added) {
    if (added) {
      scoreWorker_->triggerUpdate(coll);
    }
    return added;
  });
}

Future<bool> RelevanceServer::addNegativeDocumentToClassifier(unique_ptr<string> collId, unique_ptr<string> docId) {
  auto coll = *collId;
  auto doc = *docId;
  LOG(INFO) << "adding negative document to " << coll << " : " << doc;
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->addNegativeDocumentToClassifier(coll, doc).then([this, coll](bool added) {
    if (added) {
      scoreWorker_->triggerUpdate(coll);
    }
    return added;
  });
}
Future<bool> RelevanceServer::removeDocumentFromClassifier(unique_ptr<string> collId, unique_ptr<string> docId) {
  auto collDb = persistence_->getClassifierDb().lock();
  auto coll = *collId;
  auto doc = *docId;
  return collDb->removeDocumentFromClassifier(coll, doc).then([this, coll](bool removed){
    if (removed) {
      scoreWorker_->triggerUpdate(coll);
    }
    return removed;
  });
}

Future<bool> RelevanceServer::recompute(unique_ptr<string> collId) {
  auto coll = *collId;
  LOG(INFO) << "recomputing classifier centroid: " << coll;
  return scoreWorker_->recompute(coll);
}

Future<unique_ptr<vector<string>>> RelevanceServer::listClassifiers() {
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->listClassifiers().then([](vector<string> res) {
    return std::move(std::make_unique<vector<string>>(res));
  });
}

Future<unique_ptr<vector<string>>> RelevanceServer::listDocuments() {
  auto docDb = persistence_->getDocumentDb().lock();
  return docDb->listDocuments().then([](vector<string> res) {
    return std::move(std::make_unique<vector<string>>(res));
  });
}

Future<int> RelevanceServer::getClassifierSize(unique_ptr<string> collId) {
  auto collDb = persistence_->getClassifierDb().lock();
  return collDb->getClassifierDocumentCount(*collId);
}
