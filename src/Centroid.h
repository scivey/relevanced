#pragma once
#include <vector>

#include <cmath>
#include <glog/logging.h>
#include <eigen3/Eigen/Dense>
#include "Tfidf.h"
#include "Article.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "util.h"
namespace {
  using namespace std;
}


class Centroid {
protected:
  Tfidf *tfidf_;
  Eigen::VectorXd center_;
  vector<ProcessedDocument*> articles_;
  bool centerInitialized_ {false};
  Eigen::VectorXd getSV() {
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
public:
  Centroid(vector<ProcessedDocument*> articles, Tfidf *tfidf): articles_(articles), tfidf_(tfidf) {}
  double score(ProcessedDocument *article) {
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
  double score(Article *article) {
    ProcessedDocument processed = article->toProcessedDocument();
    return score(&processed);
  }
  bool isRelevant(ProcessedDocument *article) {
    return score(article) > 0.2;
  }
  void evalRelevance(ProcessedDocument *article) {
    double rel = score(article);
    bool result = isRelevant(article);
  }
  double test(const vector<ProcessedDocument*> &goodArticles, const vector<ProcessedDocument*> &badArticles) {
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

  ProcessedCentroid toProcsesedCentroid() {
    ProcessedCentroid result(getSV(), tfidf_->toNewProcessedTfidf());
    return result;
  }

  ProcessedCentroid* toNewProcessedCentroid() {
    return new ProcessedCentroid(getSV(), tfidf_->toNewProcessedTfidf());
  }
};