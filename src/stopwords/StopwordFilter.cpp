#include <string>
#include <set>
#include "english_stopwords.h"
#include "StopwordFilter.h"

using namespace std;

namespace stopwords {

const set<string> punctuationSet = {
  "'",
  "''",
  ",",
  ";",
  ".",
  "?",
  "!",
  "[",
  "]",
  "(",
  ")",
  "{",
  "}",
  ":",
  "\"",
  "\"\"",
  " ",
  "  ",
  "\t",
  "\r",
  "\n"
};

bool isPunctuation(const string &word) {
  return punctuationSet.find(word) != punctuationSet.end();
}

const set<char> digitCharSet = {
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9'
};

bool isDigit(char c) {
  return digitCharSet.find(c) != digitCharSet.end();
}

bool isNumeric(const string &word) {
  for(size_t i = 0; i < word.size(); i++) {
    char c = word.at(i);
    if (!isDigit(c) && c != '.') {
      return false;
    }
  }
  return true;
}

bool StopwordFilter::isStopword(const string &word) {
  return (word.size() <= 2) || isEnglishStopword(word) || isPunctuation(word) || isNumeric(word);
}

} // stopwords
