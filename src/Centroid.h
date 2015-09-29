#pragma once
#include <string>
#include <memory>
#include "ProcessedDocument.h"

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include "serialization/serializers.h"
#include "util.h"

class Centroid {
public:
  std::string id;
  std::map<std::string, double> scores;
  double magnitude {0};
  Centroid();
  Centroid(std::string);
  Centroid(std::string, std::map<std::string, double>, double);
  double score(ProcessedDocument *document);
};

namespace folly {
  template<>
  struct DynamicConstructor<Centroid> {
    static folly::dynamic construct(const Centroid &centroid) {
      auto scores = folly::toDynamic(centroid.scores);
      folly::dynamic self = folly::dynamic::object;
      self["id"] = centroid.id;
      self["magnitude"] = centroid.magnitude;
      self["scores"] = scores;
      return self;
    }
  };

  template<>
  struct DynamicConverter<Centroid> {
    static Centroid convert(const folly::dynamic &dyn) {
      auto scores = folly::convertTo<std::map<std::string, double>>(dyn["scores"]);
      auto magnitude = folly::convertTo<double>(dyn["magnitude"]);
      auto id = folly::convertTo<std::string>(dyn["id"]);
      return Centroid(id, std::move(scores), magnitude);
    }
  };
}

namespace serialization {
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

}