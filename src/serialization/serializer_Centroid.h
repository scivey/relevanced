#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include <folly/json.h>
#include "serialization/serializers.h"
#include "serialization/serializer_details.h"
#include "models/WordVector.h"
#include "models/Centroid.h"
#include "gen-cpp2/RelevancedProtocol_types.h"

namespace folly {

using relevanced::models::Centroid;
using relevanced::models::WordVector;

template <>
struct DynamicConstructor<Centroid> {
  static folly::dynamic construct(const Centroid &doc) {
    auto wordVec = folly::toDynamic(doc.wordVector);
    folly::dynamic self = folly::dynamic::object;
    self["id"] = doc.id;
    return self;
  }
};

template <>
struct DynamicConverter<Centroid> {
  static Centroid convert(const folly::dynamic &dyn) {
    auto wordVec = folly::convertTo<WordVector>(dyn["wordVector"]);
    auto id = folly::convertTo<std::string>(dyn["id"]);
    return Centroid(id, wordVec);
  }
};
} // folly


namespace relevanced {
namespace serialization {

using models::WordVector;
using models::Centroid;


template <>
struct BinarySerializer<Centroid> {
  static void serialize(std::string &result, Centroid &target) {
    thrift_protocol::CentroidDTO docDto;
    docDto.id = target.id;
    docDto.wordVector.scores = target.wordVector.scores;
    docDto.wordVector.magnitude = target.wordVector.magnitude;
    docDto.wordVector.documentWeight = target.wordVector.documentWeight;
    serialization::thriftBinarySerialize(result, docDto);
  }
};

template <>
struct BinaryDeserializer<Centroid> {
  static void deserialize(std::string &data, Centroid *result) {
    thrift_protocol::CentroidDTO docDto;
    serialization::thriftBinaryDeserialize(data, docDto);
    result->id = docDto.id;
    result->wordVector.scores = docDto.wordVector.scores;
    result->wordVector.magnitude = docDto.wordVector.magnitude;
    result->wordVector.documentWeight = docDto.wordVector.documentWeight;
  }
};


template <>
struct JsonSerializer<Centroid> {
  static void serialize(std::string &result, Centroid *document) {
    auto dynSelf = folly::toDynamic<Centroid>(*document);
    folly::fbstring js = folly::toJson(dynSelf);
    result = js.toStdString();
  }
};

template <>
struct JsonDeserializer<Centroid> {
  static void deserialize(const std::string &js, Centroid *result) {
    auto dynSelf = folly::parseJson(js);
    *result = folly::convertTo<Centroid>(dynSelf);
  }
};

} // serialization
} // relevanced
