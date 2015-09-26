#pragma once
#include <glog/logging.h>
#include <folly/Format.h>
#include <cmath>
#include <vector>
#include <memory>
#include "util.h"
#include "ProcessedDocument.h"

#include "Tfidf.h"
#include "Centroid.h"

namespace {
  using namespace std;
  using namespace folly;
  using util::UniquePointer;
}

class CentroidFactory {
protected:
  shared_ptr<Tfidf> tfidf_;
public:
  CentroidFactory(std::vector<shared_ptr<ProcessedDocument>> articles) {
    tfidf_.reset(new Tfidf(articles));
  };
  std::shared_ptr<Centroid> makeCentroid(std::vector<shared_ptr<ProcessedDocument>> articles) {
    return std::make_shared<Centroid>(articles, tfidf_);
  }
};