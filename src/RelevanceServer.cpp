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

using namespace std;
using namespace folly;
using util::UniquePointer;

RelevanceServer::RelevanceServer(
  shared_ptr<RelevanceScoreWorkerIf> scoreWorker,
  shared_ptr<DocumentProcessorIf> docProcessor,
  shared_ptr<persistence::PersistenceServiceIf> persistenceSv
): scoreWorker_(scoreWorker), docProcessor_(docProcessor), persistence_(persistenceSv) {}

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
  return scoreWorker_->getRelevanceForText(*collId, *text);
}

Future<unique_ptr<string>> RelevanceServer::createDocument(unique_ptr<string> text) {
  return internalCreateDocumentWithID(util::getUuid(), *text);
}

Future<unique_ptr<string>> RelevanceServer::internalCreateDocumentWithID(string id, string text) {
  Document doc(id, text);
  LOG(INFO) << "creating document: " << id.substr(0, 15) << "  |   " << text.substr(0, 20);
  auto processed = docProcessor_->processNew(doc);
  return persistence_->getDocumentDb().lock()->saveDocument(processed).then([id](bool saved) {
    auto uniq = std::make_unique<string>(id);
    return uniq;
  });
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
    auto uniq = std::make_unique<string>(proced.value()->toJson());
    return std::move(uniq);
  });
}

Future<bool> RelevanceServer::createCollection(unique_ptr<string> collId) {
  auto id = *collId;
  LOG(INFO) << "creating collection: " << id;
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->createCollection(id);
}

Future<bool> RelevanceServer::deleteCollection(unique_ptr<string> collId) {
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->deleteCollection(*collId);
}

Future<unique_ptr<vector<string>>> RelevanceServer::listCollectionDocuments(unique_ptr<string> collId) {
  auto id = *collId;
  LOG(INFO) << "listing documents for: " << id;
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->listCollectionDocs(id).then([id](vector<string> docIds) {
    LOG(INFO) << "listCollectionDocs: returning " << docIds.size() << " for " << id;
    return std::move(std::make_unique<vector<string>>(docIds));
  });
}

Future<bool> RelevanceServer::addPositiveDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) {
  auto coll = *collId;
  auto doc = *docId;
  LOG(INFO) << "adding positive document to " << coll << " : " << doc;
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->addPositiveDocToCollection(coll, doc).then([this, coll](bool added) {
    if (added) {
      scoreWorker_->triggerUpdate(coll);
    }
    return added;
  });
}

Future<bool> RelevanceServer::addNegativeDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) {
  auto coll = *collId;
  auto doc = *docId;
  LOG(INFO) << "adding negative document to " << coll << " : " << doc;
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->addNegativeDocToCollection(coll, doc).then([this, coll](bool added) {
    if (added) {
      scoreWorker_->triggerUpdate(coll);
    }
    return added;
  });
}
Future<bool> RelevanceServer::removeDocumentFromCollection(unique_ptr<string> collId, unique_ptr<string> docId) {
  auto collDb = persistence_->getCollectionDb().lock();
  auto coll = *collId;
  auto doc = *docId;
  return collDb->removeDocFromCollection(coll, doc).then([this, coll](bool removed){
    if (removed) {
      scoreWorker_->triggerUpdate(coll);
    }
    return removed;
  });
}

Future<bool> RelevanceServer::recompute(unique_ptr<string> collId) {
  auto coll = *collId;
  LOG(INFO) << "recomputing collection centroid: " << coll;
  return scoreWorker_->recompute(coll);
}

Future<unique_ptr<vector<string>>> RelevanceServer::listCollections() {
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->listCollections().then([](vector<string> res) {
    return std::move(std::make_unique<vector<string>>(res));
  });
}

Future<unique_ptr<vector<string>>> RelevanceServer::listDocuments() {
  auto docDb = persistence_->getDocumentDb().lock();
  return docDb->listDocuments().then([](vector<string> res) {
    return std::move(std::make_unique<vector<string>>(res));
  });
}

Future<int> RelevanceServer::getCollectionSize(unique_ptr<string> collId) {
  auto collDb = persistence_->getCollectionDb().lock();
  return collDb->getCollectionDocCount(*collId);
}
