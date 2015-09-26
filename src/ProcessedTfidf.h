#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <eigen3/Eigen/Sparse>
#include <folly/dynamic.h>
#include <folly/DynamicConverter.h>
#include <folly/Conv.h>
#include <folly/json.h>

#include "ProcessedDocument.h"
#include "util.h"

namespace {
  using namespace std;
  using namespace folly;

}

class ProcessedTfidf {
protected:
  map<string, size_t> documentCounts_;
  map<string, size_t> keyIndices_;
  vector<string> sortedKeys_;
public:
  ProcessedTfidf(map<string, size_t> docCounts, map<string, size_t> keyIndices, vector<string> sortedKeys)
    : documentCounts_(docCounts), keyIndices_(keyIndices), sortedKeys_(sortedKeys) {}

  ProcessedTfidf(map<string, size_t> documentCounts): documentCounts_(documentCounts) {
    sortedKeys_ = std::move(util::getSortedKeys(documentCounts));
    for (size_t i = 0; i < sortedKeys_.size(); i++) {
      keyIndices_[sortedKeys_.at(i)] = i;
    }
  }

  size_t getCorpusSize() {
    return sortedKeys_.size();
  }

  map<string, double> getNormalizedDocCounts(ProcessedDocument *article) {
    return article->getTfidfWordCounts(documentCounts_);
  }

  Eigen::SparseVector<double> tfVecOfArticle(ProcessedDocument *article) {
    size_t size = sortedKeys_.size();
    Eigen::SparseVector<double> vec(size);
    for (auto &elem: article->normalizedWordCounts) {
      if (keyIndices_.find(elem.first) != keyIndices_.end()) {
        vec.insert(keyIndices_[elem.first]) = elem.second;
      }
    }
    return vec;
  }

  Eigen::SparseVector<double> tfidfVecOfArticle(ProcessedDocument *article) {
    size_t size = sortedKeys_.size();
    Eigen::SparseVector<double> vec(size);
    for (auto &elem: getNormalizedDocCounts(article)) {
      vec.insert(keyIndices_[elem.first]) = elem.second;
    }
    return vec;
  }

  dynamic asDynamic_() {
    dynamic counts = folly::toDynamic(documentCounts_);
    dynamic self = dynamic::object;
    self["documentCounts"] = counts;
    return self;
  }

  string toJson() {
    fbstring js = folly::toJson(asDynamic_());
    return js.toStdString();
  }


  static std::shared_ptr<ProcessedTfidf> newFromDynamic(dynamic &d) {
    auto docCounts = d["documentCounts"];
    auto counts = folly::convertTo<map<string, size_t>>(docCounts);
    return std::make_shared<ProcessedTfidf>(counts);
  }

  static std::shared_ptr<ProcessedTfidf> newFromJson(const string &js) {
    auto dyn = folly::parseJson(js);
    return newFromDynamic(dyn);
  }

  static ProcessedTfidf fromDynamic(dynamic &d) {
    auto docCounts = d["documentCounts"];
    auto counts = folly::convertTo<map<string, size_t>>(docCounts);
    ProcessedTfidf result(counts);
    return result;
  }

  static ProcessedTfidf fromJson(const string &js) {
    auto dyn = folly::parseJson(js);
    return fromDynamic(dyn);
  }
};
