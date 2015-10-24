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

WordVector::WordVector() {}

WordVector::WordVector(unordered_map<string, double> scores, double magnitude)
    : scores(scores), magnitude(magnitude) {}

WordVector::WordVector(unordered_map<string, double> scores,
                       double magnitude,
                       double docWeight)
    : scores(scores), magnitude(magnitude), documentWeight(docWeight) {}

double WordVector::score(const unordered_map<string, double> &otherScores,
                         double otherMagnitude) {
  double dotProd = 0.0;
  for (auto &elem : otherScores) {
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

double WordVector::score(ProcessedDocument *other) {
  double dotProd = 0.0;
  for (auto &elem: other->scoredWords) {
    string key = elem.word;
    auto selfScore = scores.find(key);
    if (selfScore == scores.end()) {
      continue;
    }
    dotProd += (elem.score * selfScore->second);
  }
  return dotProd / (magnitude * other->magnitude);
}


} // models
} // relevanced
