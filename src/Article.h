#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "stopwords.h"
#include "util.h"

namespace {
  using namespace std;
}

class Article {
protected:
public:
  enum class Subject {
    MATH, POLITICS
  };
  string title_;
  string url_;
  string text_;
  Subject subject_;
  vector<string> nonStopwords_;
  map<string, size_t> wordCounts_;
  map<string, double> normalizedWordCounts_;

  Article(const string &title, const string &url, Subject subject, const string &text):
    title_(title), url_(url), subject_(subject), text_(text) {}

  vector<string>& getNonStopwords() {
    if (nonStopwords_.size() == 0) {
      nonStopwords_ = std::move(stopwords::getNonStopwords(text_));
    }
    return nonStopwords_;
  }

  map<string, size_t>& getWordCounts() {
    if (wordCounts_.size() == 0) {
      wordCounts_ = std::move(util::countWords(getNonStopwords()));
    }
    return wordCounts_;
  }

  map<string, double>& getNormalizedWordCounts() {
    if (normalizedWordCounts_.size() == 0) {
      map<string, size_t> &counts = getWordCounts();
      double tokenCount = (double) getNonStopwords().size();
      for (auto &elem: counts) {
        normalizedWordCounts_[elem.first] = ((double) elem.second) / tokenCount;
      }
    }
    return normalizedWordCounts_;
  }

  bool hasWord(const string &word) {
    return getNormalizedWordCount(word) != 0;
  }

  map<string, double> getTfidfWordCounts(const map<string, size_t> &docCounts) {
    map<string, double> output;
    for (auto &elem: getNormalizedWordCounts()) {
      if (docCounts.find(elem.first) != docCounts.end()) {
        output[elem.first] = getNormalizedWordCount(elem.first) / ((double) elem.second);
      }
    }
    return output;
  }

  double getNormalizedWordCount(const string &word) {
    map<string, double> &wordCounts = getNormalizedWordCounts();
    if (wordCounts.find(word) == wordCounts.end()) {
      return 0.0;
    }
    return wordCounts[word];
  }
};
