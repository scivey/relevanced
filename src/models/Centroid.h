#pragma once
#include <string>
#include <memory>

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "serialization/serializers.h"
#include "models/ProcessedDocument.h"
#include "gen-cpp2/TextRelevance_types.h"
#include "util/util.h"

namespace relevanced {
namespace models {

class Centroid {
public:
  size_t documentCount {0};
  std::string id;
  std::map<std::string, double> scores;
  double magnitude {0};
  Centroid();
  Centroid(std::string);
  Centroid(std::string, std::map<std::string, double>, double);
  double score(const std::map<std::string, double> &otherScores, double otherMagnitude);
  double score(ProcessedDocument *document);
  double score(Centroid *centroid);
};

} // models
} // relevanced

namespace folly {
  template<>
  struct DynamicConstructor<relevanced::models::Centroid> {
    static folly::dynamic construct(const relevanced::models::Centroid &centroid) {
      auto scores = folly::toDynamic(centroid.scores);
      folly::dynamic self = folly::dynamic::object;
      self["id"] = centroid.id;
      self["magnitude"] = centroid.magnitude;
      self["scores"] = scores;
      return self;
    }
  };

  template<>
  struct DynamicConverter<relevanced::models::Centroid> {
    static relevanced::models::Centroid convert(const folly::dynamic &dyn) {
      auto scores = folly::convertTo<std::map<std::string, double>>(dyn["scores"]);
      auto magnitude = folly::convertTo<double>(dyn["magnitude"]);
      auto id = folly::convertTo<std::string>(dyn["id"]);
      return relevanced::models::Centroid(id, std::move(scores), magnitude);
    }
  };
} // folly


namespace relevanced {
namespace serialization {

  using models::Centroid;

  template<>
  struct BinarySerializer<Centroid> {
    static void serialize(std::string &result, Centroid &target) {
      services::WordVector wordVec;
      wordVec.id = target.id;
      wordVec.scores = target.scores;
      wordVec.documentCount = target.documentCount;
      wordVec.magnitude = target.magnitude;
      serialization::thriftBinarySerialize(result, wordVec);
    }
  };

  template<>
  struct BinaryDeserializer<Centroid> {
    static void deserialize(std::string &data, Centroid *result) {
      services::WordVector wordVec;
      serialization::thriftBinaryDeserialize(data, wordVec);
      result->id = wordVec.id;
      result->magnitude = wordVec.magnitude;
      result->documentCount = wordVec.documentCount;
      result->scores = std::move(wordVec.scores);
    }
  };


  template<>
  struct JsonSerializer<Centroid> {
    static std::string serialize(Centroid *centroid) {
      auto dynSelf = folly::toDynamic<Centroid>(*centroid);
      folly::fbstring js = folly::toJson(dynSelf);
      return js.toStdString();
    }
  };

  template<>
  struct JsonDeserializer<Centroid> {
    static Centroid deserialize(const std::string &js) {
      auto dynSelf = folly::parseJson(js);
      return folly::convertTo<Centroid>(dynSelf);
    }
  };

} // serialization
} // relevanced