#include <vector>
#include <string>
#include <sstream>

#include <mitie/conll_tokenizer.h>
#include "Tokenizer.h"

using namespace std;

namespace tokenizer {

vector<string> Tokenizer::tokenize(const string &text) {
    istringstream textStream(text);
    mitie::conll_tokenizer tokenStream(textStream);
    vector<string> tokens;
    string token;
    while (tokenStream(token)) {
        tokens.push_back(token);
    }
    return tokens;
}

size_t Tokenizer::getTokenCount(const string &text) {
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
