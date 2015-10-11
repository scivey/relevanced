#pragma once

#include <string>
#include "models/WordVector.h"
#include <folly/Optional.h>

namespace relevanced {
namespace models {

class ProcessedDocument {
public:
  std::string id;
  folly::Optional<std::string> sha1Hash;
  WordVector wordVector;
  uint64_t created {0};
  uint64_t updated {0};
  ProcessedDocument(){}
  ProcessedDocument(std::string id): id(id){}
  ProcessedDocument(std::string id, WordVector wordVec): id(id), wordVector(wordVec){}
  ProcessedDocument(std::string id, std::map<std::string, double> scores, double mag):
    id(id),  wordVector(scores, mag) {}
};

} // models
} // relevanced
