#include <vector>
#include <unordered_map>
#include <cmath>
#include "text_util/WordAccumulator.h"
#include "models/ProcessedDocument.h"
#include "text_util/ScoredWord.h"
#include "text_util/StringView.h"

using namespace std;

namespace relevanced {
namespace text_util {

using models::ProcessedDocument;

WordAccumulator::WordAccumulator(size_t sizeHint) {
  scoredWords_.reserve(sizeHint);
}

void WordAccumulator::add(const StringView &word) {
  totalWords_++;
  auto existingIndex = wordsByStr_.find(word);
  if (existingIndex == wordsByStr_.end()) {
    ScoredWord scored(word.base, (uint8_t) word.len);
    scoredWords_.push_back(scored);
    wordsByStr_.insert(make_pair(word, scoredWords_.size() - 1));
  } else {
    auto existing = &scoredWords_.at(existingIndex->second);
    existing->score += 1;
  }
}

void WordAccumulator::build() {
  double dTotal = (double) totalWords_;
  magnitude_ = 0.0;
  for (auto &elem: scoredWords_) {
    double normalized = elem.score / dTotal;
    elem.score = normalized;
    magnitude_ += pow(normalized, 2);
  }
  magnitude_ = sqrt(magnitude_);
}

double WordAccumulator::getMagnitude() {
  return magnitude_;
}

vector<ScoredWord>&& WordAccumulator::getScores() {
  return std::move(scoredWords_);
}



} // text_util
} // relevanced

