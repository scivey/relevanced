#pragma once
#include <string>
#include <map>
#include <folly/FBString.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

namespace {
  using namespace std;
  using namespace folly;
}

class ProcessedDocument {
public:
  map<string, double> wordCounts;
  string id;
  ProcessedDocument(const string& id): id(id){}
  ProcessedDocument(const string& id, map<string, double> counts): id(id), wordCounts(counts){}

  void addCount(string word, double count) {
    wordCounts[word] = count;
  }
  dynamic asDynamic_() {
    dynamic counts = dynamic::object;
    for(auto &elem: wordCounts) {
      dynamic key = folly::toDynamic(elem.first);
      dynamic val = folly::toDynamic(elem.second);
      counts[key] = val;
    }
    dynamic self = dynamic::object;
    dynamic ident = folly::toDynamic(id);
    self["id"] = ident;
    self["wordCounts"] = counts;
    return self;
  }
  string toJson() {
    fbstring js = folly::toJson(asDynamic_());
    return js.toStdString();
  }
  static ProcessedDocument fromDynamic(dynamic &d) {
    string id = folly::convertTo<string>(d["id"]);
    map<string, double> wordCounts;
    auto counts = d["wordCounts"];
    for (auto &k: counts.keys()) {
      string key = folly::convertTo<string>(k);
      double val = folly::convertTo<double>(counts[k]);
      wordCounts[key] = val;
    }
    ProcessedDocument doc(id);
    doc.wordCounts = wordCounts;
    return doc;
  }
  static ProcessedDocument fromJson(const string &js) {
    auto dyn = folly::parseJson(js);
    return fromDynamic(dyn);
  }
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

