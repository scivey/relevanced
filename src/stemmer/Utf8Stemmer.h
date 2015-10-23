#pragma once

#include "stemmer/StemmerIf.h"

struct sb_stemmer;

namespace relevanced {
namespace stemmer {

class Utf8Stemmer: public StemmerIf {
protected:
  struct sb_stemmer *stemmer_;
public:
  Utf8Stemmer();
  size_t getStemPos(const char *toStem, size_t length) override;
  ~Utf8Stemmer();
};

} // text_util
} // relevanced
