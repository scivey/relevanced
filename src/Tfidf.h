#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include "Article.h"
#include "util.h"

namespace {
  using namespace std;
}

class Tfidf {
protected:
  vector<Article*> articles_;
  map<string, size_t> documentCounts_;
  map<string, size_t> keyIndices_;
  vector<string> sortedKeys_;
public:
  Tfidf(vector<Article*> articles): articles_(articles){}
  size_t getCorpusSize() {
    getDocumentCounts();
    return sortedKeys_.size();
  }
  map<string, size_t>& getDocumentCounts() {
    if (documentCounts_.size() == 0) {
      util::Counter<string> counter;
      map<string, size_t> counts;
      for (auto &art: articles_) {
        for (auto &artWord: art->getWordCounts()) {
          counter.incr(artWord.first);
        }
      }
      documentCounts_ = std::move(counter.counts);
      for (auto &elem: documentCounts_) {
        sortedKeys_.push_back(elem.first);
      }
      sort(sortedKeys_.begin(), sortedKeys_.end());
      for (size_t i = 0; i < sortedKeys_.size(); i++) {
        keyIndices_[sortedKeys_.at(i)] = i;
      }
    }
    return documentCounts_;
  }
  map<string, double> getNormalizedDocCounts(Article *article) {
    return article->getTfidfWordCounts(getDocumentCounts());
  }
  Eigen::VectorXd tfVecOfArticle(Article *article) {
    getDocumentCounts();
    size_t size = sortedKeys_.size();
    Eigen::VectorXd vec(size);
    for (size_t i = 0; i < size; i++) {
      vec(i) = 0.0;
    }
    for (auto &elem: article->getNormalizedWordCounts()) {
      if (keyIndices_.find(elem.first) != keyIndices_.end()) {
        vec(keyIndices_[elem.first]) = elem.second;
      }
    }
    return vec;
  }
  Eigen::VectorXd tfidfVecOfArticle(Article *article) {
    getDocumentCounts();
    size_t size = sortedKeys_.size();
    Eigen::VectorXd vec(size);
    for (size_t i = 0; i < size; i++) {
      vec(i) = 0.0;
    }
    // for (auto &elem: article->getNormalizedWordCounts()) {
    //   if (keyIndices_.find(elem.first) != keyIndices_.end()) {
    //     vec(keyIndices_[elem.first]) = elem.second;
    //   }
    // }
    for (auto &elem: getNormalizedDocCounts(article)) {
      vec(keyIndices_[elem.first]) = elem.second;
    }
    return vec;
  }
};
