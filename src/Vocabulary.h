#pragma once
#include <map>
#include <string>
#include <eigen3/Eigen/Sparse>
#include <folly/dynamic.h>
#include <folly/DynamicConverter.h>
#include <folly/Conv.h>
#include <folly/json.h>
#include "serialization/serializers.h"
#include "ProcessedDocument.h"
#include "util.h"

class Vocabulary {
public:
  std::map<std::string, size_t> wordIndices_;
  Vocabulary();
  Vocabulary(std::map<std::string, size_t> wordIndices);
  size_t size();
  Eigen::SparseVector<double> vecOfDocument(ProcessedDocument *document);
};


namespace folly {
  template<>
  struct DynamicConstructor<Vocabulary> {
    static folly::dynamic construct(const Vocabulary &vocabulary) {
      auto counts = folly::toDynamic(vocabulary.wordIndices_);
      folly::dynamic self = folly::dynamic::object;
      self["wordIndices"] = counts;
      return self;
    }
  };

  template<>
  struct DynamicConverter<Vocabulary> {
    static Vocabulary convert(const folly::dynamic &dyn) {
      auto countsTemp = dyn["wordIndices"];
      auto counts = folly::convertTo<std::map<std::string, size_t>>(countsTemp);
      Vocabulary vocabulary(std::move(counts));
      return vocabulary;
    }
  };

}

namespace serialization {
  template<>
  struct JsonSerializer<Vocabulary> {
    static std::string serialize(Vocabulary *vocabulary) {
      auto dynSelf = folly::toDynamic<Vocabulary>(*vocabulary);
      folly::fbstring js = folly::toJson(dynSelf);
      return js.toStdString();
    }
  };

  template<>
  struct JsonDeserializer<Vocabulary> {
    static Vocabulary deserialize(const std::string &js) {
      auto dyn = folly::parseJson(js);
      return folly::convertTo<Vocabulary>(dyn);
    }
  };
}