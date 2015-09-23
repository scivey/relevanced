#pragma once
#include <string>
#include <map>
#include <folly/FBString.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

class ProcessedDocument {
public:
  std::map<std::string, double> normalizedWordCounts;
  std::string id;
  ProcessedDocument(const std::string& id): id(id){}
  ProcessedDocument(const std::string& id, std::map<std::string, double> counts): id(id), normalizedWordCounts(counts){}
  void addCount(std::string word, double count);
  bool hasWord(const std::string &word);
  std::map<std::string, double> getTfidfWordCounts(const std::map<std::string, size_t> &docCounts);
  double getNormalizedWordCount(const std::string &word);
  folly::dynamic asDynamic_();
  std::string toJson();
  static ProcessedDocument fromDynamic(folly::dynamic &d);
  static ProcessedDocument* newFromDynamic(folly::dynamic &d);
  static ProcessedDocument fromJson(const std::string &js);
  static ProcessedDocument* newFromJson(const std::string &js);
};

