#include "text_util/ScoredWord.h"

#include <cstring>

namespace relevanced {
namespace text_util {

ScoredWord::ScoredWord(const char *wordSource, uint8_t length) {
  if (length > 23) {
    length = 23;
  }
  strncpy(word, wordSource, length);
  if (length > 22) {
    word[22] = '\0';
  } else {
    word[length] = '\0';
  }
  char cLen = (char) length;
  word[23] = cLen;
  score = 1.0;
}

size_t ScoredWord::getSize() {
  char cLen = word[23];
  size_t len = (size_t) cLen;
  return len;
}

} // text_util
} // relevanced
