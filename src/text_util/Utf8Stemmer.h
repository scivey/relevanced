#pragma once

#include <string>
#include <cstring>
#include "libstemmer.h"

struct sb_stemmer;

namespace relevanced {
namespace text_util {

class Utf8Stemmer {
protected:
  struct sb_stemmer *stemmer_;
public:
  Utf8Stemmer();
  size_t getStemPos(const char *toStem, size_t length);
  ~Utf8Stemmer();
};

} // text_util
} // relevanced
