#include <string>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "ProcessedDocument.h"
#include "Centroid.h"
#include "util.h"
using namespace std;
using namespace folly;

Centroid::Centroid(){}

Centroid::Centroid(string id): id(id) {}

Centroid::Centroid(string id, map<string, double> scores, double magnitude)
  : id(id), scores(scores), magnitude(magnitude) {}

double Centroid::score(ProcessedDocument *document) {
  double dotProd = 0.0;
  for (auto &elem: document->normalizedWordCounts) {
    auto selfScore = scores.find(elem.first);
    if (selfScore == scores.end()) {
      continue;
    }
    dotProd += (elem.second * selfScore->second);
  }
  return dotProd / (magnitude * document->magnitude);
}
