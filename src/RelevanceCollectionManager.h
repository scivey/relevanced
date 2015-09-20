#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>

#include "util.h"
#include "Article.h"
#include "CentroidFactory.h"

namespace {
  using namespace std;
}

template<typename T>
vector<T> vecOfSet(const set<T> &t) {
  vector<T> output;
  for (auto &elem: t) {
    output.push_back(elem);
  }
  return output;
}

class RelevanceCollection {
protected:
  string id_;
  map<string, Article*> articleIndex_;
  set<string> goodArticles_;
  set<string> badArticles_;
  set<string> allArticles_;

  CentroidFactory *cFactory_ {nullptr};
  Centroid *centroid_ {nullptr};
  Article* makeArt(string text) {
    auto id = util::getUuid();
    return makeArt(id, text);
  }
  Article* makeArt(string docId, string text) {
    auto art = new Article(
      docId, text
    );
    return art;
  }
public:
  RelevanceCollection(string id): id_(id) {}
  bool addPositiveDocument(Article *art) {
    LOG(INFO) << "adding good article: " << art->text_.substr(0, 50);
    if (articleIndex_.find(art->id_) != articleIndex_.end()) {
      return false;
    }
    articleIndex_[art->id_] = art;
    goodArticles_.insert(art->id_);
    allArticles_.insert(art->id_);
    return true;
  }
  bool addNegativeDocument(Article *art) {
    LOG(INFO) << "adding bad article: " << art->text_.substr(0, 50);
    if (articleIndex_.find(art->id_) != articleIndex_.end()) {
      return false;
    }
    articleIndex_[art->id_] = art;
    badArticles_.insert(art->id_);
    allArticles_.insert(art->id_);
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
    vector<Article*> goodVec;
    for (auto &elem: goodArticles_) {
      goodVec.push_back(articleIndex_[elem]);
    }
    vector<Article*> allVec;
    for (auto &elem: allArticles_) {
      allVec.push_back(articleIndex_[elem]);
    }
    cFactory_ = new CentroidFactory(allVec);
    centroid_ = cFactory_->makeCentroid(goodVec);
    LOG(INFO) << "creating new centroid";
  }
  double getRelevance(Article *article) {
    if (centroid_ == nullptr) {
      LOG(INFO) << "no centroid; returning default 0.0";
      return 0.0;
    }
    return centroid_->score(article);
  }
  double getRelevance(string text) {
    Article article("no-id", text);
    return getRelevance(&article);
  }
  int64_t getSize() {
    return allArticles_.size();
  }
  vector<string> listDocumentIds() {
    return vecOfSet(allArticles_);
  }
};

class RelevanceCollectionManager {
protected:
  map<string, RelevanceCollection*> collections_;
  map<string, Article*> documents_;
public:
  RelevanceCollectionManager(){}
  double getRelevanceForDoc(string collId, string docId) {
    if (documents_.find(docId) == documents_.end()) {
      return -999.0;
    }
    if (collections_.find(collId) == collections_.end()) {
      return -999.0;
    }
    auto doc = documents_[docId];
    return collections_[collId]->getRelevance(doc);
  }
  double getRelevanceForText(string collectionId, string text) {
    if (collections_.find(collectionId) == collections_.end()) {
      return -999.0;
    }
    return collections_[collectionId]->getRelevance(text);
  }
  bool createCollection(string id) {
    if (collections_.find(id) != collections_.end()) {
      return false;
    }
    collections_.insert(make_pair(id, new RelevanceCollection(id)));
    return true;
  }
  bool deleteCollection(string id) {
    if (collections_.find(id) != collections_.end()) {
      return false;
    }
    delete collections_[id];
    collections_.erase(id);
    return true;
  }
  bool deleteDocument(string id) {
    if (documents_.find(id) == documents_.end()) {
      return false;
    }
    for (auto &collPair: collections_) {
      auto coll = collPair.second;
      if (coll->containsDocument(id)) {
        coll->removeDocument(id);
      }
    }
    delete documents_[id];
    documents_.erase(id);
    return true;
  }
  vector<string> listDocuments() {
    vector<string> docIds;
    for (auto &docPair: documents_) {
      docIds.push_back(docPair.first);
    }
    return docIds;
  }
  bool addPositiveDocumentToCollection(string collectionId, string docId) {
    if (collections_.find(collectionId) == collections_.end()) {
      return false;
    }
    if (documents_.find(docId) == documents_.end()) {
      return false;
    }
    collections_[collectionId]->addPositiveDocument(documents_[docId]);
    return true;
  }
  bool addNegativeDocumentToCollection(string collectionId, string docId) {
    if (collections_.find(collectionId) == collections_.end()) {
      return false;
    }
    if (documents_.find(docId) == documents_.end()) {
      return false;
    }
    collections_[collectionId]->addNegativeDocument(documents_[docId]);
    return true;
  }
  string createDocument(string text) {
    auto art = new Article(util::getUuid(), text);
    documents_[art->id_] = art;
    return art->id_;
  }
  bool createDocumentWithId(string id, string text) {
    if (documents_.find(id) != documents_.end()) {
      return false;
    }
    auto art = new Article(id, text);
    documents_[id] = art;
    return true;
  }
  string addNewNegativeDocumentToCollection(string collectionId, string text) {
    auto artId = createDocument(text);
    addNegativeDocumentToCollection(collectionId, artId);
    return artId;
  }
  string addNewPositiveDocumentToCollection(string collectionId, string text) {
    auto artId = createDocument(text);
    addPositiveDocumentToCollection(collectionId, artId);
    return artId;
  }
  bool removeDocumentFromCollection(string collectionId, string docId) {
    if (collections_.find(collectionId) == collections_.end()) {
      return false;
    }
    if (documents_.find(docId) == documents_.end()) {
      return false;
    }
    return collections_[collectionId]->removeDocument(docId);
  }
  string getDocument(string docId) {
    if (documents_.find(docId) == documents_.end()) {
      return "";
    }
    return documents_[docId]->text_;
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
  vector<string> listCollectionDocuments(string collId) {
    if (collections_.find(collId) == collections_.end()) {
      vector<string> res;
      return res;
    }
    return collections_[collId]->listDocumentIds();
  }
  vector<string> listUnassociatedDocuments() {
    set<string> unassociated;
    for (auto &dPair: documents_) {
      auto docId = dPair.first;
      bool anyHasDoc = false;
      for (auto &cPair: collections_) {
        auto coll = cPair.second;
        if (coll->containsDocument(docId)) {
          anyHasDoc = true;
          break;
        }
      }
      if (!anyHasDoc) {
        unassociated.insert(docId);
      }
    }
    return vecOfSet(unassociated);
  }
};

