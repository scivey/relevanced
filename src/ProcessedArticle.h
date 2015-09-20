#pragma once
#include <string>
#include <sstream>
#include <map>

namespace {
  using namespace std;
}

class ProcessedArticle {
protected:
public:
  string id;
  map<string, double> normalizedWordCounts;
  ProcessedArticle(string id, map<string, double> wordCounts)
    : id(id), normalizedWordCounts(wordCounts){}

  bool hasWord(const string &word) {
    return getNormalizedWordCount(word) != 0;
  }
  map<string, double> getTfidfWordCounts(const map<string, size_t> &docCounts) {
    map<string, double> output;
    for (auto &elem: normalizedWordCounts) {
      if (docCounts.find(elem.first) != docCounts.end()) {
        double dCount = (double) docCounts.at(elem.first);
        output[elem.first] = elem.second / dCount;
      }
    }
    return output;
  }

  double getNormalizedWordCount(const string &word) {
    if (normalizedWordCounts.find(word) == normalizedWordCounts.end()) {
      return 0.0;
    }
    return normalizedWordCounts[word];
  }
};
