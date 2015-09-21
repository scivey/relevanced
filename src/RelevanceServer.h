#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include "gen-cpp2/Relevance.h"
#include "RelevanceWorker.h"
#include "RelevanceCollectionManager.h"


namespace {
  using namespace std;
  using namespace folly;
}

class RelevanceServer: public services::RelevanceSvIf {
  RelevanceCollectionManager *manager_;
  RelevanceWorker *worker_;
public:
  RelevanceServer(RelevanceCollectionManager *manager): manager_(manager) {
    worker_ = new RelevanceWorker(manager_);

  }
  void ping() {}
  Future<double> future_getRelevanceForDoc(unique_ptr<string> collId, unique_ptr<string> docId) {
    return worker_->getRelevanceForDoc(*collId, *docId);
  }
  Future<double> future_getRelevanceForText(unique_ptr<string> collId, unique_ptr<string> text) override {
    return worker_->getRelevanceForText(*collId, *text);
  }
  Future<unique_ptr<string>> future_createDocument(unique_ptr<string> text) override {
    return worker_->createDocument(*text).then([](string id) {
      auto uniq = std::make_unique<string>(id);
      return std::move(uniq);
    });
  }
  Future<bool> future_createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) override {
    return worker_->createDocumentWithId(*id, *text);
  }
  Future<bool> future_deleteDocument(unique_ptr<string> id) override {
    return worker_->deleteDocument(*id);
  }
  Future<unique_ptr<string>> future_getDocument(unique_ptr<string> id) override {
    return worker_->getDocument(*id).then([](string doc) {
      auto uniq = std::make_unique<string>(doc);
      return std::move(uniq);
    });
  }
  Future<bool> future_createCollection(unique_ptr<string> collId) override {
    return worker_->createCollection(*collId);
  }
  Future<bool> future_deleteCollection(unique_ptr<string> collId) override {
    return worker_->deleteCollection(*collId);
  }
  Future<unique_ptr<vector<string>>> future_listCollectionDocuments(unique_ptr<string> collId) override {
    return worker_->listCollectionDocuments(*collId).then([](vector<string> res) {
      return std::move(std::make_unique<vector<string>>(res));
    });
  }
  Future<bool> future_addPositiveDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    return worker_->addPositiveDocumentToCollection(*collId, *docId);
  }
  Future<bool> future_addNegativeDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    return worker_->addNegativeDocumentToCollection(*collId, *docId);
  }
  Future<bool> future_removeDocumentFromCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    return worker_->removeDocumentFromCollection(*collId, *docId);
  }
  Future<unique_ptr<string>> future_addNewPositiveDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> text) override {
    return worker_->addNewPositiveDocumentToCollection(*collId, *text).then([](string docId) {
      return std::move(std::make_unique<string>(docId));
    });
  }
  Future<unique_ptr<string>> future_addNewNegativeDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> text) override {
    return worker_->addNewNegativeDocumentToCollection(*collId, *text).then([](string docId) {
      return std::move(std::make_unique<string>(docId));
    });
  }
  Future<bool> future_recompute(unique_ptr<string> collId) override {
    return worker_->recompute(*collId);
  }
  Future<unique_ptr<vector<string>>> future_listCollections() override {
    return worker_->listCollections().then([](vector<string> res) {
      return std::move(std::make_unique<vector<string>>(res));
    });
  }
  Future<unique_ptr<vector<string>>> future_listDocuments() override {
    return worker_->listDocuments().then([](vector<string> res) {
      return std::move(std::make_unique<vector<string>>(res));
    });
  }
  Future<unique_ptr<vector<string>>> future_listUnassociatedDocuments() override {
    return worker_->listUnassociatedDocuments().then([](vector<string> res) {
      return std::move(std::make_unique<vector<string>>(res));
    });
  }
  Future<int> future_getCollectionSize(unique_ptr<string> collId) override {
    return worker_->getCollectionSize(*collId);
  }
};