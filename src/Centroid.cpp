#include <vector>
#include <cmath>
#include <glog/logging.h>
#include <eigen3/Eigen/Dense>

#include "Centroid.h"
#include "Tfidf.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "util.h"

using namespace std;

Eigen::VectorXd Centroid::getSV() {
  size_t size = tfidf_->getCorpusSize();
  if (!centerInitialized_) {
    Eigen::VectorXd midVec(size);
    for(size_t i = 0; i < size; i++) {
      midVec(i) = 0.0;
    }
    for (auto article: articles_) {
      auto artVec = tfidf_->tfVecOfArticle(article);
      midVec += artVec;
    }
    midVec = midVec / size;
    center_ = std::move(midVec);
  }
  return center_;
}
double Centroid::score(ProcessedDocument *article) {
  auto support = getSV();
  auto artVec = tfidf_->tfVecOfArticle(article);
  double dotProd = 0.0;
  size_t corpusSize = tfidf_->getCorpusSize();
  for (size_t i = 0; i < corpusSize; i++) {
    dotProd += (support(i) * artVec(i));
  }
  double mag1 = util::vectorMag(support, corpusSize);
  double mag2 = util::vectorMag(artVec, corpusSize);
  return dotProd / (mag1 * mag2);
}
bool Centroid::isRelevant(ProcessedDocument *article) {
  return score(article) > 0.2;
}
double Centroid::test(const vector<ProcessedDocument*> &goodArticles, const vector<ProcessedDocument*> &badArticles) {
  size_t total = goodArticles.size() + badArticles.size();
  size_t mistakes = 0;
  for (auto article: goodArticles) {
    if (!isRelevant(article)) {
      mistakes++;
    }
  }
  for(auto article: badArticles) {
    if (isRelevant(article)) {
      mistakes++;
    }
  }
  return 1.0 - ((double) mistakes) / ((double) goodArticles.size() + badArticles.size());
}

ProcessedCentroid* Centroid::toNewProcessedCentroid() {
  return new ProcessedCentroid(getSV(), tfidf_->toNewProcessedTfidf());
}
