#pragma once
#include <vector>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include "Tfidf.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "util.h"
class Centroid {
protected:
  shared_ptr<Tfidf> tfidf_;
  Eigen::SparseVector<double> center_;
  std::vector<std::shared_ptr<ProcessedDocument>> articles_;
  bool centerInitialized_ {false};
  Eigen::SparseVector<double> getSV();
public:
  Centroid(std::vector<std::shared_ptr<ProcessedDocument>> articles, shared_ptr<Tfidf> tfidf): articles_(articles), tfidf_(tfidf) {}
  double score(std::shared_ptr<ProcessedDocument> article);
  double score(ProcessedDocument* article);
  bool isRelevant(std::shared_ptr<ProcessedDocument> article);
  bool isRelevant(ProcessedDocument* article);
  std::shared_ptr<ProcessedCentroid> toNewProcessedCentroid();
};