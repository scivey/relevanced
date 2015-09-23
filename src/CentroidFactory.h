#pragma once
#include <glog/logging.h>
#include <folly/Format.h>
#include <cmath>
#include <vector>
#include "ProcessedDocument.h"

#include "Tfidf.h"
#include "Centroid.h"

namespace {
  using namespace std;
  using namespace folly;
}

class CentroidFactory {
protected:
  vector<ProcessedDocument*> articles_;
  Tfidf *tfidf_;
public:
  CentroidFactory(vector<ProcessedDocument*> articles): articles_(articles) {
    tfidf_ = new Tfidf(articles_);
  };
  Centroid* makeCentroid(vector<ProcessedDocument*> articles) {
    return new Centroid(articles, tfidf_);
  }
};