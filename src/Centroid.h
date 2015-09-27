#pragma once
#include <string>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include "Vocabulary.h"
#include "ProcessedDocument.h"

#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include "serialization/serializers.h"
#include "util.h"

class Centroid {
public:
  Eigen::SparseVector<double> center_;
  std::shared_ptr<Vocabulary> vocabulary_;
  Centroid();
  Centroid(Eigen::SparseVector<double> center, std::shared_ptr<Vocabulary> vocabulary);
  double score(ProcessedDocument *document);
};

namespace folly {
  template<>
  struct DynamicConstructor<Centroid> {
    static folly::dynamic construct(const Centroid &centroid) {
      folly::dynamic self = folly::dynamic::object;
      folly::dynamic center = folly::dynamic::object;
      for (Eigen::SparseVector<double>::InnerIterator it(centroid.center_); it; ++it) {
        string i = folly::to<std::string>(it.index());
        dynamic k = i;
        dynamic d = it.value();
        center[k] = d;
      }
      auto vocab = folly::toDynamic<Vocabulary>(*centroid.vocabulary_.get());
      self["center"] = center;
      self["vocabulary"] = vocab;
      return self;
    }
  };

  template<>
  struct DynamicConverter<Centroid> {
    static Centroid convert(const folly::dynamic &dyn) {
      auto dynVocab = dyn["vocabulary"];
      auto vocabulary = folly::convertTo<Vocabulary>(dynVocab);
      auto finalVocab = std::make_shared<Vocabulary>();
      finalVocab->wordIndices_ = std::move(vocabulary.wordIndices_);

      auto size = finalVocab->size();
      Eigen::SparseVector<double> center(size);
      auto tempDict = folly::convertTo<map<size_t, double>>(dyn["center"]);
      for (auto &elem: tempDict) {
        center.insert(elem.first) = elem.second;
      }
      Centroid centroid(std::move(center), finalVocab);
      return centroid;
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