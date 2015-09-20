#include <vector>
#include <string>
#include <sstream>

#include <mitie/conll_tokenizer.h>
#include "WhitespaceTokenizer.h"

using namespace std;

namespace tokenizer {

vector<string> WhitespaceTokenizer::tokenize(const string &text) {
    istringstream textStream(text);
    mitie::conll_tokenizer tokenStream(textStream);
    vector<string> tokens;
    string token;
    while (tokenStream(token)) {
        tokens.push_back(token);
    }
    return tokens;
}

size_t WhitespaceTokenizer::getTokenCount(const string &text) {
    size_t count = 0;
    istringstream textStream(text);
    mitie::conll_tokenizer tokenStream(textStream);
    string token;
    while (tokenStream(token)) {
        count++;
    }
    return count;
}

} // tokenizer
