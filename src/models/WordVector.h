#pragma once
#include <string>
#include <memory>
#include <unordered_map>


#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "models/ProcessedDocument.h"
#include "serialization/serializers.h"
#include "util/util.h"

namespace relevanced {
namespace models {

class WordVector {
 public:
  std::unordered_map<std::string, double> scores;
  double magnitude{0.0};
  double documentWeight{1.0};

  WordVector();

  WordVector(
    std::unordered_map<std::string, double>,
    double magnitude
  );

  WordVector(
    std::unordered_map<std::string, double>,
    double magnitude,
    double docWeight
  );

  double score(
    const std::unordered_map<std::string, double> &otherScores,
    double otherMagnitude
  );

  double score(WordVector *other);

  double score(ProcessedDocument *other);
};

} // models
} // relevanced
