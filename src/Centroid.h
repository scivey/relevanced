#pragma once
#include <vector>
#include <eigen3/Eigen/Dense>
#include "Tfidf.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"

class Centroid {
protected:
  Tfidf *tfidf_;
  Eigen::VectorXd center_;
  std::vector<ProcessedDocument*> articles_;
  bool centerInitialized_ {false};
  Eigen::VectorXd getSV();
public:
  Centroid(std::vector<ProcessedDocument*> articles, Tfidf *tfidf): articles_(articles), tfidf_(tfidf) {}
  double score(ProcessedDocument *article);
  bool isRelevant(ProcessedDocument *article);
  void evalRelevance(ProcessedDocument *article);
  double test(const std::vector<ProcessedDocument*> &goodArticles, const std::vector<ProcessedDocument*> &badArticles);
  ProcessedCentroid* toNewProcessedCentroid();
};