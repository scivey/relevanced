#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>

#include "util.h"
#include "Article.h"
#include "ProcessedDocument.h"
#include "CentroidFactory.h"
#include "persistence/CollectionDB.h"
#include "persistence/DocumentDB.h"

namespace {
  using namespace std;
}

class RelevanceCollection {
protected:
  string id_;
  map<string, ProcessedDocument*> articleIndex_;
  set<string> goodArticles_;
  set<string> badArticles_;
  set<string> allArticles_;

  CentroidFactory *cFactory_ {nullptr};
  Centroid *centroid_ {nullptr};
public:
  RelevanceCollection(string id): id_(id) {}
  bool addPositiveDocument(ProcessedDocument *art) {
    LOG(INFO) << "adding good article: " << art->id;
    if (articleIndex_.find(art->id) != articleIndex_.end()) {
      return false;
    }
    articleIndex_[art->id] = art;
    goodArticles_.insert(art->id);
    allArticles_.insert(art->id);
    return true;
  }
  bool addNegativeDocument(ProcessedDocument *art) {
    LOG(INFO) << "adding bad article: " << art->id;
    if (articleIndex_.find(art->id) != articleIndex_.end()) {
      return false;
    }
    articleIndex_[art->id] = art;
    badArticles_.insert(art->id);
    allArticles_.insert(art->id);
    return true;
  }
  bool removeDocument(string id) {
    if (articleIndex_.find(id) == articleIndex_.end()) {
      return false;
    }
    articleIndex_.erase(id);
    if (badArticles_.find(id) != badArticles_.end()) {
      badArticles_.erase(id);
    }
    if (goodArticles_.find(id) != goodArticles_.end()) {
      goodArticles_.erase(id);
    }
    if (allArticles_.find(id) != allArticles_.end()) {
      allArticles_.erase(id);
    }
    return true;
  }
  bool containsDocument(string docId) {
    return allArticles_.find(docId) != allArticles_.end();
  }
  void recompute() {
    if (cFactory_ != nullptr) {
      delete cFactory_;
    }
    if (centroid_ != nullptr) {
      delete centroid_;
    }
    vector<ProcessedDocument*> goodVec;
    for (auto &elem: goodArticles_) {
      goodVec.push_back(articleIndex_[elem]);
    }
    vector<ProcessedDocument*> allVec;
    for (auto &elem: allArticles_) {
      allVec.push_back(articleIndex_[elem]);
    }
    cFactory_ = new CentroidFactory(allVec);
    centroid_ = cFactory_->makeCentroid(goodVec);
    LOG(INFO) << "creating new centroid";
  }
  double getRelevance(ProcessedDocument *article) {
    if (centroid_ == nullptr) {
      LOG(INFO) << "no centroid; returning default 0.0";
      return 0.0;
    }
    return centroid_->score(article);
  }
  double getRelevance(string text) {
    Article article("no-id", text);
    ProcessedDocument processed = article.toProcessedDocument();
    return getRelevance(&processed);
  }
  int64_t getSize() {
    return allArticles_.size();
  }
  vector<string> listDocumentIds() {
    return util::vecOfSet(allArticles_);
  }
};
