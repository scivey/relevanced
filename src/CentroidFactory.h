#pragma once
#include <glog/logging.h>
#include <folly/Format.h>
#include <cmath>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "Article.h"
#include "Tfidf.h"
#include "Centroid.h"

namespace {
  using namespace std;
  using namespace folly;
}

class CentroidFactory {
protected:
  vector<Article*> articles_;
  bool centerInitialized_ {false};
  Eigen::VectorXd center_;
  Tfidf *tfidf_;
public:
  CentroidFactory(vector<Article*> articles): articles_(articles) {
    tfidf_ = new Tfidf(articles_);
  };
  Centroid* makeCentroid(vector<Article*> articles) {
    return new Centroid(articles, tfidf_);
  }
  // Eigen::VectorXd getSV() {
  //   size_t size = tfidf_->getCorpusSize();
  //   if (!centerInitialized_) {
  //     Eigen::VectorXd midVec(size);
  //     for(size_t i = 0; i < size; i++) {
  //       midVec(i) = 0.0;
  //     }
  //     for (auto article: articles_) {
  //       auto artVec = tfidf_->vecOfArticle(article);
  //       midVec += artVec;
  //     }
  //     midVec = midVec / size;
  //     center_ = std::move(midVec);
  //   }
  //   return center_;
  // }
  // double score(Article *article) {
  //   auto support = getSV();
  //   auto artVec = tfidf_->vecOfArticle(article);
  //   double dotProd = 0.0;
  //   size_t corpusSize = tfidf_->getCorpusSize();
  //   for (size_t i = 0; i < corpusSize; i++) {
  //     dotProd += (support(i) * artVec(i));
  //   }
  //   double mag1 = vectorMag(support, corpusSize);
  //   double mag2 = vectorMag(artVec, corpusSize);
  //   return dotProd / (mag1 * mag2);
  // }
  // void evalRelevance(Article *article) {
  //   double rel = score(article);
  //   bool result = (rel > 0.5);
  //   LOG(INFO) << "relevance: " << article->title_ << "  = " << rel << "  -> " << result;
  // }
};