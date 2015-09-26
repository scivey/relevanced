#pragma once
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <eigen3/Eigen/Sparse>
#include <glog/logging.h>
#include "ProcessedDocument.h"
#include "ProcessedTfidf.h"

#include "util.h"

namespace {
  using namespace std;
}

class Tfidf {
protected:
  vector<shared_ptr<ProcessedDocument>> articles_;
  map<string, size_t> documentCounts_;
  map<string, size_t> keyIndices_;
  vector<string> sortedKeys_;
public:
  Tfidf(vector<shared_ptr<ProcessedDocument>> articles): articles_(articles){}
  size_t getCorpusSize() {
    getDocumentCounts();
    return sortedKeys_.size();
  }
  map<string, size_t>& getDocumentCounts() {
    if (documentCounts_.size() == 0) {
      util::Counter<string> counter;
      map<string, size_t> counts;
      for (auto art: articles_) {
        for (auto artWord: art->normalizedWordCounts) {
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
  Eigen::SparseVector<double> tfVecOfArticle(ProcessedDocument *article) {
    getDocumentCounts();
    size_t size = sortedKeys_.size();
    Eigen::SparseVector<double> vec(size);
    for (auto &elem: article->normalizedWordCounts) {
      if (keyIndices_.find(elem.first) != keyIndices_.end()) {
        vec.insert(keyIndices_[elem.first]) = elem.second;
      }
    }
    return vec;
  }
  Eigen::SparseVector<double> tfVecOfArticle(std::shared_ptr<ProcessedDocument> article) {
    return tfVecOfArticle(article.get());
  }
  Eigen::SparseVector<double> tfidfVecOfArticle(ProcessedDocument *article) {
    getDocumentCounts();
    size_t size = sortedKeys_.size();
    Eigen::SparseVector<double> vec(size);
    for (auto &elem: getNormalizedDocCounts(article)) {
      vec.insert(keyIndices_[elem.first]) = elem.second;
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
  std::shared_ptr<ProcessedTfidf> toNewProcessedTfidf() {
    return std::make_shared<ProcessedTfidf>(
      getDocumentCounts(), keyIndices_, sortedKeys_
    );
  }
};
