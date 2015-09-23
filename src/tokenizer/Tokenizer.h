#pragma once
#include <string>
#include <vector>

namespace tokenizer {

class TokenizerIf {
public:
  virtual std::vector<std::string> tokenize(const std::string &text) = 0;
  virtual size_t getTokenCount(const std::string &text) = 0;
};

class Tokenizer: public TokenizerIf {
  std::vector<std::string> tokenize(const std::string &text) override;
  size_t getTokenCount(const std::string &text) override;
};

}