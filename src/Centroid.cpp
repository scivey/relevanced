#include <vector>
#include <cmath>
#include <memory>
#include <glog/logging.h>
#include <eigen3/Eigen/Sparse>

#include "Centroid.h"
#include "Tfidf.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "util.h"

using util::UniquePointer;
using namespace std;

Eigen::SparseVector<double> Centroid::getSV() {
  size_t size = tfidf_->getCorpusSize();
  if (!centerInitialized_) {
    Eigen::SparseVector<double> midVec(size);
    for (auto article: articles_) {
      auto artVec = tfidf_->tfVecOfArticle(article);
      midVec += artVec;
    }
    midVec = midVec / size;
    center_ = std::move(midVec);
  }
  return center_;
}
double Centroid::score(ProcessedDocument* article) {
  auto support = getSV();
  auto artVec = tfidf_->tfVecOfArticle(article);
  double dotProd = util::sparseDot(artVec, support);
  return dotProd;
}

double Centroid::score(shared_ptr<ProcessedDocument> article) {
  return score(article.get());
}

bool Centroid::isRelevant(ProcessedDocument* article) {
  return score(article) > 0.2;
}
bool Centroid::isRelevant(shared_ptr<ProcessedDocument> article) {
  return isRelevant(article.get());
}

shared_ptr<ProcessedCentroid> Centroid::toNewProcessedCentroid() {
  return std::make_shared<ProcessedCentroid>(getSV(), tfidf_->toNewProcessedTfidf());
}
