#pragma once
#include <string>
#include <memory>

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "serialization/serializers.h"
#include "gen-cpp2/TextRelevance_types.h"
#include "util/util.h"

namespace relevanced {
namespace models {

class WordVector {
public:
  std::string id;
  std::map<std::string, double> scores;
  double magnitude {0.0};
  double documentWeight {1.0};
  WordVector();
  WordVector(std::string);
  WordVector(std::string, std::map<std::string, double>, double magnitude);
  WordVector(std::string, std::map<std::string, double>, double magnitude, double docWeight);
  double score(const std::map<std::string, double> &otherScores, double otherMagnitude);
  double score(WordVector *other);
};

} // models
} // relevanced

namespace folly {

  using relevanced::models::WordVector;

  template<>
  struct DynamicConstructor<WordVector> {
    static folly::dynamic construct(const WordVector &wordVec) {
      auto scores = folly::toDynamic(wordVec.scores);
      folly::dynamic self = folly::dynamic::object;
      self["id"] = wordVec.id;
      self["magnitude"] = wordVec.magnitude;
      self["scores"] = scores;
      self["documentWeight"] = wordVec.documentWeight;
      return self;
    }
  };

  template<>
  struct DynamicConverter<WordVector> {
    static WordVector convert(const folly::dynamic &dyn) {
      auto scores = folly::convertTo<std::map<std::string, double>>(dyn["scores"]);
      auto id = folly::convertTo<std::string>(dyn["id"]);
      auto magnitude = folly::convertTo<double>(dyn["magnitude"]);
      auto weight = folly::convertTo<double>(dyn["documentWeight"]);
      return WordVector(id, std::move(scores), magnitude, weight);
    }
  };
} // folly


namespace relevanced {
namespace serialization {

  using models::WordVector;

  template<>
  struct BinarySerializer<WordVector> {
    static void serialize(std::string &result, WordVector &target) {
      services::WordVectorDTO vecDto;
      vecDto.id = target.id;
      vecDto.scores = target.scores;
      vecDto.documentWeight = target.documentWeight;
      vecDto.magnitude = target.magnitude;
      serialization::thriftBinarySerialize(result, vecDto);
    }
  };

  template<>
  struct BinaryDeserializer<WordVector> {
    static void deserialize(std::string &data, WordVector *result) {
      services::WordVectorDTO vecDto;
      serialization::thriftBinaryDeserialize(data, vecDto);
      result->id = vecDto.id;
      result->magnitude = vecDto.magnitude;
      result->documentWeight = vecDto.documentWeight;
      result->scores = std::move(vecDto.scores);
    }
  };


  template<>
  struct JsonSerializer<WordVector> {
    static std::string serialize(WordVector *wordVector) {
      auto dynSelf = folly::toDynamic<WordVector>(*wordVector);
      folly::fbstring js = folly::toJson(dynSelf);
      return js.toStdString();
    }
  };

  template<>
  struct JsonDeserializer<WordVector> {
    static WordVector deserialize(const std::string &js) {
      auto dynSelf = folly::parseJson(js);
      return folly::convertTo<WordVector>(dynSelf);
    }
  };

} // serialization
} // relevanced