#include <string>
#include <memory>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "util/util.h"

namespace relevanced {
namespace models {

using namespace std;
using namespace folly;

Centroid::Centroid(){}

Centroid::Centroid(string id): id(id) {}

Centroid::Centroid(string id, map<string, double> scores, double magnitude)
  : id(id), scores(scores), magnitude(magnitude) {}

double Centroid::score(const map<string, double> &otherScores, double otherMagnitude) {
  double dotProd = 0.0;
  for (auto &elem: otherScores) {
    auto selfScore = scores.find(elem.first);
    if (selfScore == scores.end()) {
      continue;
    }
    dotProd += (elem.second * selfScore->second);
  }
  return dotProd / (magnitude * otherMagnitude);
}

double Centroid::score(ProcessedDocument *document) {
  return score(document->normalizedWordCounts, document->magnitude);
}

double Centroid::score(Centroid *centroid) {
  return score(centroid->scores, centroid->magnitude);
}

} // models
} // relevanced
