#pragma once
#include <string>
#include <memory>
#include "models/ProcessedDocument.h"

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include "serialization/serializers.h"
#include "util/util.h"

namespace relevanced {
namespace models {

class Centroid {
public:
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