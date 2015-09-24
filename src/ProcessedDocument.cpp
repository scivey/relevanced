#include "ProcessedDocument.h"
#include <string>
#include <map>
#include <folly/FBString.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

using namespace std;
using namespace folly;

void ProcessedDocument::addCount(string word, double count) {
  normalizedWordCounts[word] = count;
}

bool ProcessedDocument::hasWord(const string &word) {
  return getNormalizedWordCount(word) != 0;
}

void ProcessedDocument::addTfidfWordCounts(const map<string, size_t> &docCounts, map<string, double> &output) {
  for (auto &elem: normalizedWordCounts) {
    if (docCounts.find(elem.first) != docCounts.end()) {
      double dCount = (double) docCounts.at(elem.first);
      output[elem.first] = elem.second / dCount;
    }
  }
}

map<string, double> ProcessedDocument::getTfidfWordCounts(const map<string, size_t> &docCounts) {
  map<string, double> output;
  addTfidfWordCounts(docCounts, output);
  return output;
}

double ProcessedDocument::getNormalizedWordCount(const string &word) {
  if (normalizedWordCounts.find(word) == normalizedWordCounts.end()) {
    return 0.0;
  }
  return normalizedWordCounts[word];
}

dynamic ProcessedDocument::asDynamic_() {
  dynamic counts = dynamic::object;
  for(auto &elem: normalizedWordCounts) {
    dynamic key = folly::toDynamic(elem.first);
    dynamic val = folly::toDynamic(elem.second);
    counts[key] = val;
  }
  dynamic self = dynamic::object;
  dynamic ident = folly::toDynamic(id);
  self["id"] = ident;
  self["normalizedWordCounts"] = counts;
  return self;
}

string ProcessedDocument::toJson() {
  fbstring js = folly::toJson(asDynamic_());
  return js.toStdString();
}

ProcessedDocument ProcessedDocument::fromDynamic(dynamic &d) {
  string id = folly::convertTo<string>(d["id"]);
  map<string, double> normalizedWordCounts;
  auto counts = d["normalizedWordCounts"];
  for (auto &k: counts.keys()) {
    string key = folly::convertTo<string>(k);
    double val = folly::convertTo<double>(counts[k]);
    normalizedWordCounts[key] = val;
  }
  ProcessedDocument doc(id);
  doc.normalizedWordCounts = normalizedWordCounts;
  return doc;
}

ProcessedDocument* ProcessedDocument::newFromDynamic(dynamic &d) {
  string id = folly::convertTo<string>(d["id"]);
  map<string, double> normalizedWordCounts;
  auto counts = d["normalizedWordCounts"];
  for (auto &k: counts.keys()) {
    string key = folly::convertTo<string>(k);
    double val = folly::convertTo<double>(counts[k]);
    normalizedWordCounts[key] = val;
  }
  auto doc = new ProcessedDocument(id);
  doc->normalizedWordCounts = normalizedWordCounts;
  return doc;
}
ProcessedDocument ProcessedDocument::fromJson(const string &js) {
  auto dyn = folly::parseJson(js);
  return fromDynamic(dyn);
}

ProcessedDocument* ProcessedDocument::newFromJson(const string &js) {
  auto dyn = folly::parseJson(js);
  return newFromDynamic(dyn);
}


