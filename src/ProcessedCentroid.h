#pragma once
#include <string>
#include <eigen3/Eigen/Dense>
#include "ProcessedTfidf.h"
#include "ProcessedDocument.h"
#include "Article.h"

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "util.h"
namespace {
  using namespace std;
}

class ProcessedCentroid {
protected:
  Eigen::VectorXd center_;
  ProcessedTfidf* tfidf_;
public:
  ProcessedCentroid(Eigen::VectorXd center, ProcessedTfidf* tfidf)
    : center_(center), tfidf_(tfidf) {}

  double score(const string &text) {
    Article article("no-id", text);
    auto processed = article.toProcessedDocument();
    return score(&processed);
  }

  double score(ProcessedDocument *article) {
    auto artVec = tfidf_->tfVecOfArticle(article);
    double dotProd = 0.0;
    size_t corpusSize = tfidf_->getCorpusSize();
    for (size_t i = 0; i < corpusSize; i++) {
      dotProd += (center_(i) * artVec(i));
    }
    double mag1 = util::vectorMag(center_, corpusSize);
    double mag2 = util::vectorMag(artVec, corpusSize);
    return dotProd / (mag1 * mag2);
  }

  dynamic asDynamic_() {
    dynamic self = dynamic::object;
    dynamic center = {};
    size_t corpusSize = tfidf_->getCorpusSize();
    for (size_t i = 0; i < corpusSize; i++) {
      center.push_back(center_(i));
    }
    dynamic tfidf = tfidf_->asDynamic_();
    self["center"] = center;
    self["tfidf"] = tfidf;
    return self;
  }

  string toJson() {
    fbstring js = folly::toJson(asDynamic_());
    return js.toStdString();
  }

  static ProcessedCentroid fromDynamic(dynamic &d) {
    auto tfidf = ProcessedTfidf::newFromDynamic(d["tfidf"]);
    auto size = tfidf->getCorpusSize();
    Eigen::VectorXd center(size);
    auto tempVec = folly::convertTo<vector<double>>(d["center"]);
    for(size_t i = 0; i < size; i++) {
      center(i) = tempVec.at(i);
    }
    ProcessedCentroid centroid (center, tfidf);
    return centroid;
  }

  static ProcessedCentroid* newFromDynamic(dynamic &d) {
    auto tfidf = ProcessedTfidf::newFromDynamic(d["tfidf"]);
    auto size = tfidf->getCorpusSize();
    Eigen::VectorXd center(size);
    auto tempVec = folly::convertTo<vector<double>>(d["center"]);
    for(size_t i = 0; i < size; i++) {
      center(i) = tempVec.at(i);
    }
    return new ProcessedCentroid(center, tfidf);
  }

  static ProcessedCentroid fromJson(const string &js) {
    auto dyn = folly::parseJson(js);
    return fromDynamic(dyn);
  }

  static ProcessedCentroid* newFromJson(const string &js) {
    auto dyn = folly::parseJson(js);
    return newFromDynamic(dyn);
  }

};