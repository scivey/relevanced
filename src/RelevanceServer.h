#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "RelevanceScoreWorker.h"

#include "persistence/PersistenceService.h"
#include "DocumentProcessor.h"
#include "Document.h"
#include "util.h"

namespace {
  using namespace std;
  using namespace folly;
}

class RelevanceServer: public services::RelevanceSvIf {
  shared_ptr<persistence::PersistenceServiceIf> persistence_;
  shared_ptr<DocumentProcessor> docProcessor_;
  shared_ptr<RelevanceScoreWorker> scoreWorker_;
public:
  RelevanceServer(
    shared_ptr<RelevanceScoreWorker> scoreWorker,
    shared_ptr<DocumentProcessor> docProcessor,
    shared_ptr<persistence::PersistenceServiceIf> persistenceSv
  ): scoreWorker_(scoreWorker), docProcessor_(docProcessor), persistence_(persistenceSv) {}
  void ping() {}
  Future<double> future_getRelevanceForDoc(unique_ptr<string> collId, unique_ptr<string> docId) {
    string coll = *collId;
    return persistence_->getDocumentDb().lock()->loadDocument(*docId).then([this, coll](ProcessedDocument *doc) {
      return scoreWorker_->getRelevanceForDoc(coll, doc);
    });
  }
  Future<double> future_getRelevanceForText(unique_ptr<string> collId, unique_ptr<string> text) override {
    return scoreWorker_->getRelevanceForText(*collId, *text);
  }
  Future<unique_ptr<string>> future_createDocument(unique_ptr<string> text) override {
    return future_createDocumentWithID(util::getUuid(), *text);
  }

  Future<unique_ptr<string>> future_createDocumentWithID(string id, string text) {
    Document doc(id, text);
    LOG(INFO) << "creating document: " << id.substr(0, 15) << "  |   " << text.substr(0, 20);
    auto processed = docProcessor_->processNew(doc);
    return persistence_->getDocumentDb().lock()->saveDocument(processed).then([id](bool saved) {
      auto uniq = std::make_unique<string>(id);
      return uniq;
    });
  }

  Future<unique_ptr<string>> future_createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) override {
    return future_createDocumentWithID(*id, *text);
  }

  Future<bool> future_deleteDocument(unique_ptr<string> id) override {
    return persistence_->getDocumentDb().lock()->deleteDocument(*id);
  }

  Future<unique_ptr<string>> future_getDocument(unique_ptr<string> id) override {
    auto docDb = persistence_->getDocumentDb().lock();
    return docDb->loadDocument(*id).then([](ProcessedDocument* proced) {
      auto uniq = std::make_unique<string>(proced->id);
      return std::move(uniq);
    });
  }
  Future<bool> future_createCollection(unique_ptr<string> collId) override {
    auto id = *collId;
    LOG(INFO) << "creating collection: " << id;
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->createCollection(id);
  }
  Future<bool> future_deleteCollection(unique_ptr<string> collId) override {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->deleteCollection(*collId);
  }
  Future<unique_ptr<vector<string>>> future_listCollectionDocuments(unique_ptr<string> collId) override {
    auto id = *collId;
    LOG(INFO) << "listing documents for: " << id;
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->listCollectionDocs(id).then([](vector<string> docIds) {
      return std::move(std::make_unique<vector<string>>(docIds));
    });
  }
  Future<bool> future_addPositiveDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    auto coll = *collId;
    auto doc = *docId;
    LOG(INFO) << "adding positive document to " << coll << " : " << doc;
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->addPositiveDocToCollection(coll, doc);
  }
  Future<bool> future_addNegativeDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    auto coll = *collId;
    auto doc = *docId;
    LOG(INFO) << "adding negative document to " << coll << " : " << doc;
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->addNegativeDocToCollection(coll, doc);
  }
  Future<bool> future_removeDocumentFromCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->removeDocFromCollection(*collId, *docId);
  }
  Future<bool> future_recompute(unique_ptr<string> collId) override {
    auto coll = *collId;
    LOG(INFO) << "recomputing collection centroid: " << coll;
    return scoreWorker_->recompute(coll);
  }
  Future<unique_ptr<vector<string>>> future_listCollections() override {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->listCollections().then([](vector<string> res) {
      return std::move(std::make_unique<vector<string>>(res));
    });
  }
  Future<unique_ptr<vector<string>>> future_listDocuments() override {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->listKnownDocuments().then([](vector<string> res) {
      return std::move(std::make_unique<vector<string>>(res));
    });
  }
  Future<int> future_getCollectionSize(unique_ptr<string> collId) override {
    auto collDb = persistence_->getCollectionDb().lock();
    return collDb->getCollectionDocCount(*collId);
  }
};