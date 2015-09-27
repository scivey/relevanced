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
      auto extra = result.id.find('|');
      if (extra != string::npos) {
        result.id.erase(extra);
      }
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

  template<>
  struct JsonSerializer<ProcessedDocument> {
    static std::string serialize(ProcessedDocument *doc) {
      folly::dynamic counts = folly::dynamic::object;
      for(auto &elem: doc->normalizedWordCounts) {
        dynamic key = folly::toDynamic(elem.first);
        dynamic val = folly::toDynamic(elem.second);
        counts[key] = val;
      }
      folly::dynamic self = folly::dynamic::object;
      folly::dynamic ident = folly::toDynamic(doc->id);
      self["id"] = ident;
      self["normalizedWordCounts"] = counts;
      folly::fbstring js = folly::toJson(self);
      return js.toStdString();
    }
  };

  template<>
  struct JsonDeserializer<ProcessedDocument> {
    static ProcessedDocument deserialize(const std::string &js) {
      auto dyn = folly::parseJson(js);
      auto counts = dyn["normalizedWordCounts"];
      ProcessedDocument result(
        folly::convertTo<std::string>(dyn["id"])
      );
      for (auto &k: counts.keys()) {
        string key = folly::convertTo<string>(k);
        double val = folly::convertTo<double>(counts[k]);
        result.normalizedWordCounts[key] = val;
      }
      return result;
    }
  };


}
