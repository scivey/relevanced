#pragma once
#include <glog/logging.h>
#include <folly/Format.h>
#include <cmath>
#include <vector>
#include "Article.h"
#include "ProcessedArticle.h"

#include "Tfidf.h"
#include "Centroid.h"

namespace {
  using namespace std;
  using namespace folly;
}

class CentroidFactory {
protected:
  vector<ProcessedArticle*> articles_;
  Tfidf *tfidf_;
public:
  CentroidFactory(vector<ProcessedArticle*> articles): articles_(articles) {
    tfidf_ = new Tfidf(articles_);
  };
  Centroid* makeCentroid(vector<ProcessedArticle*> articles) {
    return new Centroid(articles, tfidf_);
  }
};