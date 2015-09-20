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
  bool recompute(unique_ptr<string> collId) override {
    string cid = *collId.get();
    auto res = worker_->recompute(cid).get();
    return res;
  }
  bool addPositiveToCollection(unique_ptr<string> collId, unique_ptr<string> artId, unique_ptr<string> text) override {
    string cid = *collId.get();
    string aid = *artId.get();
    string txt = *text.get();
    auto res = worker_->addPositiveToCollection(cid, aid, txt).get();
    return res;
  }
  bool addNegativeToCollection(unique_ptr<string> collId, unique_ptr<string> artId, unique_ptr<string> text) override {
    string cid = *collId.get();
    string aid = *artId.get();
    string txt = *text.get();
    auto res = worker_->addNegativeToCollection(cid, aid, txt).get();
    return res;
  }
  bool createCollection(unique_ptr<string> collId) override {
    string cid = *collId.get();
    auto res = worker_->createCollection(cid).get();
    return res;
  }
  void ping() {}
  double getRelevance(unique_ptr<string> collId, unique_ptr<string> text) override {
    string cid = *collId.get();
    string txt = *text.get();
    auto res = worker_->getRelevance(cid, txt).get();
    return res;
  }
  void listCollections(vector<string> &result) override {
    result = std::move(worker_->listCollections().get());
  }
  int64_t getCollectionSize(unique_ptr<string> collId) override {
    string cid = *collId.get();
    auto res = worker_->getCollectionSize(cid).get();
    return res;
  }
};