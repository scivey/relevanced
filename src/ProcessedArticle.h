#pragma once
#include <string>
#include <sstream>
#include <map>

namespace {
  using namespace std;
}

class ProcessedArticle {
protected:
  string id_;
  map<string, double> normalizedWordCounts_;
public:
  ProcessedArticle(string id, map<string, double> wordCounts)
    : id_(id), normalizedWordCounts_(wordCount){}

  bool hasWord(const string &word) {
    return getNormalizedWordCount(word) != 0;
  }
  map<string, double> getTfidfWordCounts(const map<string, size_t> &docCounts) {
    map<string, double> output;
    for (auto &elem: normalizedWordCounts_) {
      if (docCounts.find(elem.first) != docCounts.end()) {
        output[elem.first] = elem.second / ((double) docCounts[elem.first]);
      }
    }
    return output;
  }

  double getNormalizedWordCount(const string &word) {
    if (normalizedWordCounts_.find(word) == normalizedWordCounts_.end()) {
      return 0.0;
    }
    return normalizedWordCounts_[word];
  }
}