#pragma once
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>

#include "Article.h"
#include "CentroidFactory.h"

namespace {
  using namespace std;
}

class RelevanceCollection {
protected:
  string id_;
  vector<Article*> goodArticles_;
  vector<Article*> badArticles_;
  vector<Article*> allArticles_;

  CentroidFactory *cFactory_ {nullptr};
  Centroid *centroid_ {nullptr};
  Article* makeArt(string docId, string text) {
    auto art = new Article(
      docId, docId, "", Article::Subject::UNKNOWN, text
    );
    return art;
  }
public:
  RelevanceCollection(string id): id_(id) {}
  bool addGoodArticle(string docId, string text) {
    LOG(INFO) << "adding good article: " << text.substr(0, 50);
    auto art = makeArt(docId, text);
    goodArticles_.push_back(art);
    allArticles_.push_back(art);
    return true;
  }
  bool addBadArticle(string docId, string text) {
    LOG(INFO) << "adding bad article: " << text.substr(0, 50);
    auto art = makeArt(docId, text);
    badArticles_.push_back(art);
    allArticles_.push_back(art);
    return true;
  }
  void recompute() {
    if (cFactory_ != nullptr) {
      delete cFactory_;
    }
    if (centroid_ != nullptr) {
      delete centroid_;
    }
    cFactory_ = new CentroidFactory(allArticles_);
    centroid_ = cFactory_->makeCentroid(goodArticles_);
    LOG(INFO) << "creating new centroid";
  }
  double score(string text) {
    if (centroid_ == nullptr) {
      LOG(INFO) << "no centroid; returning default 0.0";
      return 0.0;
    }
    Article article("no-id", "no-url", "no-title", Article::Subject::UNKNOWN, text);
    return centroid_->score(&article);
  }
  int64_t getSize() {
    return allArticles_.size();
  }
};

class RelevanceCollectionManager {
protected:
  map<string, RelevanceCollection*> collections_;
public:
  RelevanceCollectionManager(){}
  bool createCollection(string id) {
    if (collections_.find(id) != collections_.end()) {
      return false;
    }
    collections_.insert(make_pair(id, new RelevanceCollection(id)));
    return true;
  }
  double getRelevance(string collectionId, string text) {
    if (collections_.find(collectionId) == collections_.end()) {
      return -999.0;
    }
    return collections_[collectionId]->score(text);
  }
  bool addPositiveToCollection(string collectionId, string textId, string text) {
    if (collections_.find(collectionId) == collections_.end()) {
      return false;
    }
    collections_[collectionId]->addGoodArticle(textId, text);
    return true;
  }
  bool addNegativeToCollection(string collectionId, string textId, string text) {
    if (collections_.find(collectionId) == collections_.end()) {
      return false;
    }
    collections_[collectionId]->addBadArticle(textId, text);
    return true;
  }
  bool recompute(string collectionId) {
    if (collections_.find(collectionId) == collections_.end()) {
      return false;
    }
    collections_[collectionId]->recompute();
    return true;
  }
  int64_t getCollectionSize(string collectionId) {
    if (collections_.find(collectionId) == collections_.end()) {
      return 0;
    }
    return collections_[collectionId]->getSize();
  }
  vector<string> listCollections() {
    vector<string> output;
    for (auto &elem: collections_) {
      output.push_back(elem.first);
    }
    return std::move(output);
  }
};

