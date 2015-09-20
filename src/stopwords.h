#pragma once
#include <string>
#include <vector>

#include "englishStopwordSet.h"
#include "WhitespaceTokenizer.h"
#include "stringUtil.h"

namespace {
  using namespace std;
}
namespace stopwords {

bool isStopword(const std::string& word) {
  return isEnglishStopword(word);
}

bool isJunk(const std::string& word) {
  return (word.size() <= 2) || isStopword(word) || isPunctuation(word) || isNumeric(word);
}

vector<string> getNonStopwords(const string &text) {
  tokenizer::WhitespaceTokenizer tokenizer;
  vector<string> result;
  for (auto &elem: tokenizer.tokenize(text)) {
    string word = elem;
    if (stringUtil::isOnlyAscii(word)) {
      word = stringUtil::lowerCase(word);
    }
    if (!isJunk(word)) {
      result.push_back(word);
    }
  }
  return result;
}


} // stopwords
