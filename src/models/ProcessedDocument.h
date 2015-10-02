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

namespace relevanced {
namespace models {

class ProcessedDocument {
public:
  std::string id;
  std::map<std::string, double> normalizedWordCounts;
  double magnitude;
  ProcessedDocument(const std::string& id): id(id){}
  ProcessedDocument(const std::string& id, std::map<std::string, double> counts, double magnitude)
    : id(id), normalizedWordCounts(counts), magnitude(magnitude) {}
};

} // models
} // relevanced

namespace folly {
  template<>
  struct DynamicConstructor<relevanced::models::ProcessedDocument> {
    static folly::dynamic construct(const relevanced::models::ProcessedDocument &document) {
      auto scores = folly::toDynamic(document.normalizedWordCounts);
      folly::dynamic self = folly::dynamic::object;
      self["id"] = document.id;
      self["magnitude"] = document.magnitude;
      self["scores"] = scores;
      return self;
    }
  };

  template<>
  struct DynamicConverter<relevanced::models::ProcessedDocument> {
    static relevanced::models::ProcessedDocument convert(const folly::dynamic &dyn) {
      auto scores = folly::convertTo<std::map<std::string, double>>(dyn["scores"]);
      auto magnitude = folly::convertTo<double>(dyn["magnitude"]);
      auto id = folly::convertTo<std::string>(dyn["id"]);
      return relevanced::models::ProcessedDocument(id, std::move(scores), magnitude);
    }
  };
} // folly

namespace relevanced {
namespace serialization {

  using models::ProcessedDocument;

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
      auto dynSelf = folly::toDynamic<ProcessedDocument>(*doc);
      folly::fbstring js = folly::toJson(dynSelf);
      return js.toStdString();
    }
  };

  template<>
  struct JsonDeserializer<ProcessedDocument> {
    static ProcessedDocument deserialize(const std::string &js) {
      auto dynSelf = folly::parseJson(js);
      return folly::convertTo<ProcessedDocument>(dynSelf);
    }
  };
} // serialization
} // relevanced