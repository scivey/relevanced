#pragma once
#include <vector>

#include <cmath>
#include <glog/logging.h>
#include <eigen3/Eigen/Dense>
#include "Tfidf.h"
#include "Article.h"

namespace {
  using namespace std;
}

double vectorMag(const Eigen::VectorXd &vec, size_t count) {
  double accum = 0.0;
  for (size_t i = 0; i < count; i++) {
    accum += pow(vec(i), 2);
  }
  return sqrt(accum);
}

class Centroid {
protected:
  Tfidf *tfidf_;
  Eigen::VectorXd center_;
  vector<Article*> articles_;
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
  Centroid(vector<Article*> articles, Tfidf *tfidf): articles_(articles), tfidf_(tfidf) {}
  double score(Article *article) {
    auto support = getSV();
    auto artVec = tfidf_->tfVecOfArticle(article);
    double dotProd = 0.0;
    size_t corpusSize = tfidf_->getCorpusSize();
    for (size_t i = 0; i < corpusSize; i++) {
      dotProd += (support(i) * artVec(i));
    }
    double mag1 = vectorMag(support, corpusSize);
    double mag2 = vectorMag(artVec, corpusSize);
    return dotProd / (mag1 * mag2);
  }
  bool isRelevant(Article *article) {
    return score(article) > 0.2;
  }
  void evalRelevance(Article *article) {
    double rel = score(article);
    bool result = isRelevant(article);
    LOG(INFO) << "relevance: " << article->title_ << "  = " << rel << "  -> " << result;
  }
  double test(const vector<Article*> &goodArticles, const vector<Article*> &badArticles) {
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
};