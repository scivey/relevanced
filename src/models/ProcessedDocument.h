#pragma once

#include <string>
#include <vector>
#include "models/WordVector.h"
#include "text_util/ScoredWord.h"
#include <folly/Optional.h>

namespace relevanced {
namespace models {

class ProcessedDocument {
 public:
  std::string id;
  folly::Optional<std::string> sha1Hash;
  std::vector<text_util::ScoredWord> scoredWords;
  double magnitude;
  uint64_t created{0};
  uint64_t updated{0};
  ProcessedDocument() {}
  ProcessedDocument(std::string id) : id(id) {}
  ProcessedDocument(std::string id, std::vector<text_util::ScoredWord> scoredWords, double magnitude)
      : id(id), scoredWords(scoredWords), magnitude(magnitude) {}

};

} // models
} // relevanced
