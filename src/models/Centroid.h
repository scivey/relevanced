#pragma once

#include <string>
#include <unordered_map>
#include <atomic>
#include "models/WordVector.h"

namespace relevanced {
namespace models {

class Centroid {
 public:
  std::string id;
  WordVector wordVector;
  Centroid() {}
  Centroid(std::string id) : id(id) {}
  Centroid(std::string id, WordVector wordVec) : id(id), wordVector(wordVec) {}
  Centroid(std::string id, std::unordered_map<std::string, double> scores, double mag)
      : id(id), wordVector(scores, mag) {}

  template <typename T>
  double score(T* t) {
    return wordVector.score(t);
  }
};

} // models
} // relevanced
