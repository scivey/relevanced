#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>

#include "text_util/Utf8Stemmer.h"
#include "text_util/ScoredWord.h"

namespace relevanced {
namespace text_util {

class TextScorer {
protected:
  std::shared_ptr<Utf8Stemmer> stemmer_;
public:
  TextScorer(std::shared_ptr<Utf8Stemmer> stemmer);
  std::vector<ScoredWord> scoreText(const std::string& text);
};

} // text_util
} // relevanced
