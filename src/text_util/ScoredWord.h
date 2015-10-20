#pragma once

#include <cstring>
#include <cstdint>

namespace relevanced {
namespace text_util {

struct ScoredWord {
  char word[24];
  double score {1.0};
  ScoredWord(const char *wordSource, uint8_t length);
  size_t getSize();
};

} // text_util
} // relevanced
