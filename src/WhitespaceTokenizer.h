#pragma once
#include <vector>
#include <string>

namespace tokenizer {

class WhitespaceTokenizer {
public:
    std::vector<std::string> tokenize(const std::string &text);
    size_t getTokenCount(const std::string &text);
};

} // tokenizer
