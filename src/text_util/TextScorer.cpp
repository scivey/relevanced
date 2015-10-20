#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>

#include "text_util/TextScorer.h"
#include "text_util/Utf8Stemmer.h"
#include "text_util/ScoredWord.h"
#include "text_util/StringView.h"
#include "text_util/DestructiveConllTokenIterator.h"

namespace relevanced {
namespace text_util {

TextScorer::TextScorer(std::shared_ptr<Utf8Stemmer> stemmer): stemmer_(stemmer) {}

std::vector<ScoredWord> TextScorer::scoreText(const std::string& text) {
  DestructiveConllTokenIterator it(text.c_str(), text.size());
  std::vector<ScoredWord> scoredWords;
  scoredWords.reserve(200);
  std::unordered_map<StringView, size_t> wordsByStr;
  const char *cStr = text.c_str();
  size_t totalWords {0};
  std::tuple<bool, size_t, size_t> tokenOffsets;
  while (it.next(tokenOffsets)) {
    if (!std::get<0>(tokenOffsets)) {
      break;
    }
    size_t startPos = ::std::get<1>(tokenOffsets);
    size_t len = ::std::get<2>(tokenOffsets) - startPos;
    if (len < 3) {
      continue;
    }
    const char *tokenStart = cStr + startPos;
    char firstChar = *tokenStart;
    if (it.isAsciiNumeric(firstChar)) {
      continue;
    }
    if (it.isPunctuation(firstChar)) {
      continue;
    }
    len = stemmer_->getStemPos(tokenStart, len);
    if (len > 23) {
      len = 23;
    }
    totalWords++;
    StringView view(tokenStart, len);
    auto existingIndex = wordsByStr.find(view);
    if (existingIndex == wordsByStr.end()) {
      uint8_t wordLen = (uint8_t) len;
      scoredWords.emplace_back(tokenStart, wordLen);
      wordsByStr[view] = scoredWords.size() - 1;
    } else {
      auto existing = &scoredWords.at(existingIndex->second);
      existing->score += 1;
    }
  }
  return std::move(scoredWords);
}

} // text_util
} // relevanced
