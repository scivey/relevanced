#pragma once
#include <string>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include "ProcessedTfidf.h"
#include "ProcessedDocument.h"

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "util.h"

class ProcessedCentroid {
protected:
  Eigen::SparseVector<double> center_;
  std::shared_ptr<ProcessedTfidf> tfidf_;
public:
  ProcessedCentroid(Eigen::SparseVector<double> center, std::shared_ptr<ProcessedTfidf> tfidf);
  double score(ProcessedDocument *article);
  folly::dynamic asDynamic_();
  std::string toJson();
  static ProcessedCentroid fromDynamic(folly::dynamic &d);
  static ProcessedCentroid* newFromDynamic(folly::dynamic &d);
  static ProcessedCentroid fromJson(const std::string &js);
  static ProcessedCentroid* newFromJson(const std::string &js);
};