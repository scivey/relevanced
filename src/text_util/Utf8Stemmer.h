#pragma once

#include <string>
#include <cstring>
#include "libstemmer.h"

class Utf8Stemmer {
protected:
  struct sb_stemmer *stemmer_;
public:
  Utf8Stemmer() {
    stemmer_ = sb_stemmer_new("en", "UTF_8");
  }
  size_t getStemPos(const char *toStem, size_t length) {
    sb_stemmer_stem(stemmer_, (const sb_symbol*) toStem, length);
    return sb_stemmer_length(stemmer_);
  }
  size_t getStemPos(const char *toStem) {
    return getStemPos(toStem, strlen(toStem));
  }
  bool stemInPlace(char *toStem, size_t length) {
    size_t offset = getStemPos(toStem, length);
    if (offset >= (length - 1)) {
      return false;
    }
    char *pos = toStem + offset;
    *pos = '\0';
    return true;
  }
  bool stemInPlace(std::string &toStem) {
    auto pos = getStemPos(toStem.c_str(), toStem.size());
    if (pos < toStem.size()) {
      toStem.erase(pos);
      return true;
    }
    return false;
  }
  ~Utf8Stemmer() {
    sb_stemmer_delete(stemmer_);
  }
};
