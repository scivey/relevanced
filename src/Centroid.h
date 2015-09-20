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
  Eigen::VectorXd centroid_;
  double centroidMag_;
  vector<Article*> articles_;
  Eigen::VectorXd getSV() {
    size_t size = tfidf_->getCorpusSize();
    Eigen::VectorXd midVec(size);
    for(size_t i = 0; i < size; i++) {
      midVec(i) = 0.0;
    }
    for (auto article: articles_) {
      auto artVec = tfidf_->tfVecOfArticle(article);
      midVec += artVec;
    }
    midVec = midVec / articles_.size();
    return midVec;
  }
public:
  Centroid(vector<Article*> articles, Tfidf *tfidf): articles_(articles), tfidf_(tfidf){
    centroid_ = getSV();
    centroidMag_ = vectorMag(centroid_, articles.size());
    LOG(INFO) << "centroidMag_ : " << centroidMag_;
  }
  double score(Article *article) {
    auto artVec = tfidf_->tfVecOfArticle(article);
    double dotProd = 0.0;
    size_t corpusSize = tfidf_->getCorpusSize();
    for (size_t i = 0; i < corpusSize; i++) {
      dotProd += (centroid_(i) * artVec(i));
    }
    double artMag = vectorMag(artVec, corpusSize);
    LOG(INFO) << "artMag: " << artMag;
    LOG(INFO) << "dotProd: " << dotProd;
    return dotProd / (centroidMag_ * artMag);
  }
  void evalRelevance(Article *article) {
    double rel = score(article);
    bool result = (rel > 0.5);
    LOG(INFO) << "relevance: " << article->title_ << "  = " << rel << "  -> " << result;
  }
};