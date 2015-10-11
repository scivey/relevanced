#pragma once
#include <string>
#include "StemmerIf.h"

namespace relevanced {
namespace stemmer {

class PorterStemmer : public StemmerIf {
 public:
  std::string stem(const std::string &word) override;
  void stemInPlace(std::string &word) override;
};

} // stemmer
} // relevanced
