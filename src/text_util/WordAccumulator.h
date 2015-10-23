#pragma once
#include <vector>
#include <unordered_map>
#include "models/ProcessedDocument.h"
#include "text_util/ScoredWord.h"
#include "text_util/StringView.h"

namespace relevanced {
namespace text_util {

class WordAccumulator {
  size_t totalWords_ {0};
  double magnitude_ {0.0};
  std::vector<ScoredWord> scoredWords_;
  std::unordered_map<StringView, size_t> wordsByStr_;
public:
  WordAccumulator(size_t sizeHint);
  void add(const StringView &word);
  void build();
  double getMagnitude();
  std::vector<ScoredWord>&& getScores();
};

} // text_util
} // relevanced

