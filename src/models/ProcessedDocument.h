#pragma once
#include <string>
#include <map>
#include <glog/logging.h>
#include <folly/FBString.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <thrift/lib/cpp2/protocol/Serializer.h>
#include "gen-cpp2/TextRelevance_types.h"
#include "serialization/serializers.h"

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
    static void serialize(std::string &result, ProcessedDocument &target) {
      services::WordVector wordVec;
      wordVec.id = target.id;
      wordVec.scores = target.normalizedWordCounts;
      wordVec.documentCount = 1;
      wordVec.magnitude = target.magnitude;
      serialization::thriftBinarySerialize(result, wordVec);
    }
  };

  template<>
  struct BinaryDeserializer<ProcessedDocument> {
    static void deserialize(std::string &data, ProcessedDocument *result) {
      services::WordVector wordVec;
      serialization::thriftBinaryDeserialize(data, wordVec);
      result->id = wordVec.id;
      result->magnitude = wordVec.magnitude;
      result->normalizedWordCounts = std::move(wordVec.scores);
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