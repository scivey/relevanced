#include <string>
#include <set>
#include "stopwords/english_stopwords.h"
#include "stopwords/StopwordFilter.h"

using namespace std;

namespace relevanced {
namespace stopwords {

const set<string> punctuationSet = {"'",  "''",   ",", ";",  ".",  "?",  "!",
                                    "[",  "]",    "(", ")",  "{",  "}",  ":",
                                    "\"", "\"\"", " ", "  ", "\t", "\r", "\n"};

bool isPunctuation(const string &word) {
  return punctuationSet.find(word) != punctuationSet.end();
}

const set<char> digitCharSet = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

bool isDigit(char c) { return digitCharSet.find(c) != digitCharSet.end(); }

bool isNumeric(const string &word) {
  for (size_t i = 0; i < word.size(); i++) {
    char c = word.at(i);
    if (!isDigit(c) && c != '.') {
      return false;
    }
  }
  return true;
}

bool isAsciiAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isAsciiNumeric(char c) {
  return (c >= '0' && c <= '9');
}

bool isAsciiAlphaNumeric(char c) {
  return isAsciiAlpha(c) || isAsciiNumeric(c);
}

bool isAsciiWhitespace(char c) {
  return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

bool isPunctuation(char c) {
  return (c < '0' || (c < 'A' && c > '9') || (c > 'Z' && c < 'a') || c > 'z');
}

bool StopwordFilter::isStopword(const string &word) {
  if (word.size() <= 2) {
    return true;
  }
  auto first = word.at(0);
  if (isPunctuation(first) || isAsciiNumeric(first) || isAsciiWhitespace(first)) {
    return true;
  }
  return isEnglishStopword(word);
}

} // stopwords
} // relevanced
