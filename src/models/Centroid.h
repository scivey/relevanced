#pragma once

#include <string>
#include <map>
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "models/WordVector.h"

namespace relevanced {
namespace models {

class Centroid {
public:
  std::string id;
  WordVector wordVector;
  Centroid(){}
  Centroid(std::string id): id(id){}
  Centroid(std::string id, WordVector wordVec): id(id), wordVector(wordVec){}
  Centroid(std::string id, std::map<std::string, double> scores, double mag):
    id(id), wordVector(scores, mag) {}

  template<typename T>
  double score(T* t) {
    return wordVector.score(t);
  }
};

} // models
} // relevanced


namespace folly {

  using relevanced::models::Centroid;
  using relevanced::models::WordVector;

  template<>
  struct DynamicConstructor<Centroid> {
    static folly::dynamic construct(const Centroid &doc) {
      auto wordVec = folly::toDynamic(doc.wordVector);
      folly::dynamic self = folly::dynamic::object;
      self["id"] = doc.id;
      return self;
    }
  };

  template<>
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


  template<>
  struct BinarySerializer<Centroid> {
    static void serialize(std::string &result, Centroid &target) {\
      thrift_protocol::CentroidDTO docDto;
      docDto.id = target.id;
      docDto.wordVector.scores = target.wordVector.scores;
      docDto.wordVector.magnitude = target.wordVector.magnitude;
      docDto.wordVector.documentWeight = target.wordVector.documentWeight;
      serialization::thriftBinarySerialize(result, docDto);
    }
  };

  template<>
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


  template<>
  struct JsonSerializer<Centroid> {
    static std::string serialize(Centroid *document) {
      auto dynSelf = folly::toDynamic<Centroid>(*document);
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