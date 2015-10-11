#pragma once
#include <string>
#include <vector>

namespace relevanced {
namespace tokenizer {

class TokenizerIf {
 public:
  virtual std::vector<std::string> tokenize(const std::string &text) = 0;
};

class Tokenizer : public TokenizerIf {
 public:
  std::vector<std::string> tokenize(const std::string &text) override;
};
}
} // relevanced
