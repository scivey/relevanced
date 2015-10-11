#include <string>
#include <map>

#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include <folly/json.h>
#include "serialization/serializers.h"
#include "serialization/serializer_details.h"
#include "models/WordVector.h"
#include "gen-cpp2/RelevancedProtocol_types.h"

namespace folly {

using relevanced::models::WordVector;

template <>
struct DynamicConstructor<WordVector> {
  static folly::dynamic construct(const WordVector &wordVec) {
    auto scores = folly::toDynamic(wordVec.scores);
    folly::dynamic self = folly::dynamic::object;
    self["magnitude"] = wordVec.magnitude;
    self["scores"] = scores;
    self["documentWeight"] = wordVec.documentWeight;
    return self;
  }
};

template <>
struct DynamicConverter<WordVector> {
  static WordVector convert(const folly::dynamic &dyn) {
    auto scores =
        folly::convertTo<std::map<std::string, double>>(dyn["scores"]);
    auto magnitude = folly::convertTo<double>(dyn["magnitude"]);
    auto weight = folly::convertTo<double>(dyn["documentWeight"]);
    return WordVector(std::move(scores), magnitude, weight);
  }
};
} // folly


namespace relevanced {
namespace serialization {

using models::WordVector;

template <>
struct BinarySerializer<WordVector> {
  static void serialize(std::string &result, WordVector &target) {
    thrift_protocol::WordVectorDTO vecDto;
    vecDto.scores = target.scores;
    vecDto.documentWeight = target.documentWeight;
    vecDto.magnitude = target.magnitude;
    serialization::thriftBinarySerialize(result, vecDto);
  }
};

template <>
struct BinaryDeserializer<WordVector> {
  static void deserialize(std::string &data, WordVector *result) {
    thrift_protocol::WordVectorDTO vecDto;
    serialization::thriftBinaryDeserialize(data, vecDto);
    result->magnitude = vecDto.magnitude;
    result->documentWeight = vecDto.documentWeight;
    result->scores = std::move(vecDto.scores);
  }
};


template <>
struct JsonSerializer<WordVector> {
  static void serialize(std::string &result, WordVector *wordVector) {
    auto dynSelf = folly::toDynamic<WordVector>(*wordVector);
    folly::fbstring js = folly::toJson(dynSelf);
    result = js.toStdString();
  }
};

template <>
struct JsonDeserializer<WordVector> {
  static void deserialize(const std::string &js, WordVector *result) {
    auto dynSelf = folly::parseJson(js);
    *result = folly::convertTo<WordVector>(dynSelf);
  }
};

} // serialization
} // relevanced
