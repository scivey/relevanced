#include <string>
#include <memory>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "models/WordVector.h"
#include "util/util.h"

namespace relevanced {
namespace models {

using namespace std;
using namespace folly;

WordVector::WordVector(){}

WordVector::WordVector(map<string, double> scores, double magnitude)
  : scores(scores), magnitude(magnitude) {}

WordVector::WordVector(map<string, double> scores, double magnitude, double docWeight)
  : scores(scores), magnitude(magnitude), documentWeight(docWeight) {}

double WordVector::score(const map<string, double> &otherScores, double otherMagnitude) {
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

double WordVector::score(WordVector *other) {
  return score(other->scores, other->magnitude);
}

} // models
} // relevanced
