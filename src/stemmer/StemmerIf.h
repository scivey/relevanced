#pragma once
#include <string>

namespace relevanced {
namespace stemmer {

class StemmerIf {
 public:
  virtual size_t getStemPos(const char *base, size_t length) = 0;
};

} // stemmer
} // relevanced
