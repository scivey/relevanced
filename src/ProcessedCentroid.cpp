#include <string>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include "ProcessedTfidf.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "util.h"
using namespace std;
using namespace folly;

ProcessedCentroid::ProcessedCentroid(Eigen::SparseVector<double> center, std::shared_ptr<ProcessedTfidf> tfidf)
  : center_(center), tfidf_(tfidf) {}

double ProcessedCentroid::score(ProcessedDocument *article) {
  auto artVec = tfidf_->tfVecOfArticle(article);
  size_t corpusSize = tfidf_->getCorpusSize();
  map<size_t, double> indices;
  double dotProd = 0.0;
  for (Eigen::SparseVector<double>::InnerIterator it(artVec); it; ++it) {
    indices.insert(make_pair(it.index(), it.value()));
  }
  for (Eigen::SparseVector<double>::InnerIterator it(center_); it; ++it) {
    if (indices.find(it.index()) != indices.end()) {
      dotProd += (indices[it.index()] * it.value());
    }
  }
  double mag1 = util::vectorMag(center_, corpusSize);
  double mag2 = util::vectorMag(artVec, corpusSize);
  return dotProd / (mag1 * mag2);
}

dynamic ProcessedCentroid::asDynamic_() {
  dynamic self = dynamic::object;
  dynamic center = dynamic::object;
  size_t corpusSize = tfidf_->getCorpusSize();
  for (Eigen::SparseVector<double>::InnerIterator it(center_); it; ++it) {
    string i = folly::to<string>(it.index());
    dynamic k = i;
    dynamic d = it.value();
    center[k] = d;
  }
  dynamic tfidf = tfidf_->asDynamic_();
  self["center"] = center;
  self["tfidf"] = tfidf;
  return self;
}

string ProcessedCentroid::toJson() {
  fbstring js = folly::toJson(asDynamic_());
  return js.toStdString();
}

ProcessedCentroid ProcessedCentroid::fromDynamic(dynamic &d) {
  auto tfidf = ProcessedTfidf::newFromDynamic(d["tfidf"]);
  auto size = tfidf->getCorpusSize();
  Eigen::SparseVector<double> center(size);
  auto tempDict = folly::convertTo<map<size_t, double>>(d["center"]);
  for (auto &elem: tempDict) {
    center.insert(elem.first) = elem.second;
  }
  ProcessedCentroid centroid (center, tfidf);
  return centroid;
}

ProcessedCentroid* ProcessedCentroid::newFromDynamic(dynamic &d) {
  auto tfidf = ProcessedTfidf::newFromDynamic(d["tfidf"]);
  auto size = tfidf->getCorpusSize();
  Eigen::SparseVector<double> center(size);
  auto tempDict = folly::convertTo<map<size_t, double>>(d["center"]);
  for (auto &elem: tempDict) {
    center.insert(elem.first) = elem.second;
  }
  return new ProcessedCentroid(center, tfidf);
}

ProcessedCentroid ProcessedCentroid::fromJson(const string &js) {
  auto dyn = folly::parseJson(js);
  return fromDynamic(dyn);
}

ProcessedCentroid* ProcessedCentroid::newFromJson(const string &js) {
  auto dyn = folly::parseJson(js);
  return newFromDynamic(dyn);
}

