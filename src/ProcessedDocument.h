#pragma once
#include <string>
#include <map>
#include <glog/logging.h>
#include <folly/FBString.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include "serialization/serializers.h"
#include "serialization/bytes.h"


class ProcessedDocument {
public:
  std::map<std::string, double> normalizedWordCounts;
  std::string id;
  ProcessedDocument(const std::string& id): id(id){}
  ProcessedDocument(const std::string& id, std::map<std::string, double> counts): id(id), normalizedWordCounts(counts){}
  void addCount(std::string word, double count);
  bool hasWord(const std::string &word);
  void addTfidfWordCounts(const std::map<std::string, size_t> &docCounts, std::map<std::string, double> &target);
  std::map<std::string, double> getTfidfWordCounts(const std::map<std::string, size_t> &docCounts);
  double getNormalizedWordCount(const std::string &word);
  folly::dynamic asDynamic_();
  std::string toJson();
  static ProcessedDocument fromDynamic(folly::dynamic &d);
  static ProcessedDocument* newFromDynamic(folly::dynamic &d);
  static ProcessedDocument fromJson(const std::string &js);
  static ProcessedDocument* newFromJson(const std::string &js);
};


namespace serialization {
  template<>
  struct BinarySerializer<ProcessedDocument> {
    static size_t serialize(unsigned char **bytes, ProcessedDocument &target) {
      size_t sizeEstimate = 0;
      size_t elemCount = 0;
      for (auto &elem: target.normalizedWordCounts) {
        sizeEstimate += elem.first.size() + 8;
        sizeEstimate += 8;
        elemCount++;
      }
      sizeEstimate += sizeof(size_t) * 2 * elemCount;
      sizeEstimate += target.id.size() + 8;
      LOG(INFO) << "size estimate: " << sizeEstimate << " for " << target.id;
      ByteArrayWriter writer(sizeEstimate);
      writer.write<string>(target.id);
      for (auto &elem: target.normalizedWordCounts) {
        writer.write<string>(elem.first);
        writer.write<double>(elem.second);
      }
      writer.finalize();
      *bytes = writer.getData();
      return writer.getSize();
    }
  };

  template<>
  struct BinaryDeserializer<ProcessedDocument> {
    static void deserialize(unsigned char *bytes, ProcessedDocument &result) {
      ByteReader reader(bytes);
      reader.readString(result.id);
      while (!reader.isFinished()) {
        string key;
        reader.readString(key);
        auto extraChars = key.find('|');
        if (extraChars != string::npos) {
          key.erase(extraChars);
        }
        double val;
        reader.read(val);
        result.normalizedWordCounts.insert(make_pair(key, val));
      }
    }
  };
}
