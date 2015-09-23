#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include "ProcessedDocument.h"
#include "ProcessedTfidf.h"

#include "util.h"

namespace {
  using namespace std;
}

class Tfidf {
protected:
  vector<ProcessedDocument*> articles_;
  map<string, size_t> documentCounts_;
  map<string, size_t> keyIndices_;
  vector<string> sortedKeys_;
public:
  Tfidf(vector<ProcessedDocument*> articles): articles_(articles){}
  size_t getCorpusSize() {
    getDocumentCounts();
    return sortedKeys_.size();
  }
  map<string, size_t>& getDocumentCounts() {
    if (documentCounts_.size() == 0) {
      util::Counter<string> counter;
      map<string, size_t> counts;
      for (auto &art: articles_) {
        for (auto &artWord: art->normalizedWordCounts) {
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
  map<string, double> getNormalizedDocCounts(ProcessedDocument *article) {
    return article->getTfidfWordCounts(getDocumentCounts());
  }
  Eigen::VectorXd tfVecOfArticle(ProcessedDocument *article) {
    getDocumentCounts();
    size_t size = sortedKeys_.size();
    Eigen::VectorXd vec(size);
    for (size_t i = 0; i < size; i++) {
      vec(i) = 0.0;
    }
    for (auto &elem: article->normalizedWordCounts) {
      if (keyIndices_.find(elem.first) != keyIndices_.end()) {
        vec(keyIndices_[elem.first]) = elem.second;
      }
    }
    return vec;
  }
  Eigen::VectorXd tfidfVecOfArticle(ProcessedDocument *article) {
    getDocumentCounts();
    size_t size = sortedKeys_.size();
    Eigen::VectorXd vec(size);
    for (size_t i = 0; i < size; i++) {
      vec(i) = 0.0;
    }
    for (auto &elem: getNormalizedDocCounts(article)) {
      vec(keyIndices_[elem.first]) = elem.second;
    }
    return vec;
  }
  ProcessedTfidf toProcessedTfidf() {
    ProcessedTfidf processed(
      getDocumentCounts(),
      keyIndices_,
      sortedKeys_
    );
    return processed;
  }
  ProcessedTfidf* toNewProcessedTfidf() {
    return new ProcessedTfidf(
      getDocumentCounts(), keyIndices_, sortedKeys_
    );
  }
};
