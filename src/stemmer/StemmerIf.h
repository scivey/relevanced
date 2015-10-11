#pragma once
#include <string>

namespace relevanced {
namespace stemmer {

class StemmerIf {
 public:
  virtual std::string stem(const std::string &word) = 0;
  virtual void stemInPlace(std::string &word) = 0;
};

} // stemmer
} // relevanced
