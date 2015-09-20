#pragma once
#include <string>
#include <memory>
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
  double getRelevanceForDoc(unique_ptr<string> collId, unique_ptr<string> docId) {
    return worker_->getRelevanceForDoc(*collId, *docId).get();
  }
  double getRelevanceForText(unique_ptr<string> collId, unique_ptr<string> text) override {
    return worker_->getRelevanceForText(*collId, *text).get();
  }
  void createDocument(string &result, unique_ptr<string> text) override {
    result = worker_->createDocument(*text).get();
  }
  bool createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) override {
    return worker_->createDocumentWithId(*id, *text).get();
  }
  bool deleteDocument(unique_ptr<string> id) override {
    return worker_->deleteDocument(*id).get();
  }
  void getDocument(string &result, unique_ptr<string> id) override {
    result = worker_->getDocument(*id).get();
  }
  bool createCollection(unique_ptr<string> collId) override {
    auto res = worker_->createCollection(*collId).get();
    return res;
  }
  bool deleteCollection(unique_ptr<string> collId) override {
    return worker_->deleteCollection(*collId).get();
  }
  void listCollectionDocuments(vector<string> &output, unique_ptr<string> collId) override {
    output = worker_->listCollectionDocuments(*collId).get();
  }
  bool addPositiveDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    return worker_->addPositiveDocumentToCollection(*collId, *docId).get();
  }
  bool addNegativeDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    return worker_->addNegativeDocumentToCollection(*collId, *docId).get();
  }
  bool removeDocumentFromCollection(unique_ptr<string> collId, unique_ptr<string> docId) override {
    return worker_->removeDocumentFromCollection(*collId, *docId).get();
  }
  void addNewPositiveDocumentToCollection(string &result, unique_ptr<string> collId, unique_ptr<string> text) override {
    result = worker_->addNewPositiveDocumentToCollection(*collId, *text).get();
  }
  void addNewNegativeDocumentToCollection(string &result, unique_ptr<string> collId, unique_ptr<string> text) override {
    result = worker_->addNewNegativeDocumentToCollection(*collId, *text).get();
  }
  bool recompute(unique_ptr<string> collId) override {
    return worker_->recompute(*collId).get();
  }
  void listCollections(vector<string> &result) override {
    result = std::move(worker_->listCollections().get());
  }
  void listDocuments(vector<string> &result) override {
    result = std::move(worker_->listDocuments().get());
  }
  void listUnassociatedDocuments(vector<string> &result) override {
    result = std::move(worker_->listUnassociatedDocuments().get());
  }
  int64_t getCollectionSize(unique_ptr<string> collId) override {
    return worker_->getCollectionSize(*collId).get();
  }
};