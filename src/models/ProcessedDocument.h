#pragma once

#include <string>
#include "models/WordVector.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include <folly/Optional.h>

namespace relevanced {
namespace models {

class ProcessedDocument {
public:
  std::string id;
  folly::Optional<std::string> sha1Hash;
  WordVector wordVector;
  uint64_t created {0};
  uint64_t updated {0};
  ProcessedDocument(){}
  ProcessedDocument(std::string id): id(id){}
  ProcessedDocument(std::string id, WordVector wordVec): id(id), wordVector(wordVec){}
  ProcessedDocument(std::string id, std::map<std::string, double> scores, double mag):
    id(id),  wordVector(scores, mag) {}
};

} // models
} // relevanced

namespace folly {

  using relevanced::models::ProcessedDocument;
  using relevanced::models::WordVector;

  template<>
  struct DynamicConstructor<ProcessedDocument> {
    static folly::dynamic construct(const ProcessedDocument &doc) {
      auto wordVec = folly::toDynamic(doc.wordVector);
      folly::dynamic self = folly::dynamic::object;
      self["id"] = doc.id;
      self["created"] = doc.created;
      self["updated"] = doc.updated;
      if(doc.sha1Hash.hasValue()) {
        self["sha1Hash"] = doc.sha1Hash.value();
      } else {
        self["sha1Hash"] = "";
      }
      return self;
    }
  };

  template<>
  struct DynamicConverter<ProcessedDocument> {
    static ProcessedDocument convert(const folly::dynamic &dyn) {
      auto wordVec = folly::convertTo<WordVector>(dyn["wordVector"]);
      auto id = folly::convertTo<std::string>(dyn["id"]);
      ProcessedDocument result(id, wordVec);
      auto updated = folly::convertTo<uint64_t>(dyn["updated"]);
      auto created = folly::convertTo<uint64_t>(dyn["created"]);
      result.updated = updated;
      result.created = created;
      auto hash = folly::convertTo<std::string>(dyn["sha1Hash"]);
      if (hash.size() > 0) {
        result.sha1Hash.assign(hash);
      }
      return result;
    }
  };
} // folly


namespace relevanced {
namespace serialization {

  using models::WordVector;
  using models::ProcessedDocument;


  template<>
  struct BinarySerializer<ProcessedDocument> {
    static void serialize(std::string &result, ProcessedDocument &target) {\
      thrift_protocol::ProcessedDocumentDTO docDto;
      docDto.wordVector.scores = target.wordVector.scores;
      docDto.wordVector.magnitude = target.wordVector.magnitude;
      docDto.wordVector.documentWeight = target.wordVector.documentWeight;
      docDto.id = target.id;
      docDto.updated = target.updated;
      docDto.created = target.created;
      if (target.sha1Hash.hasValue()) {
        docDto.sha1Hash = target.sha1Hash.value();
      } else {
        docDto.sha1Hash = "";
      }
      serialization::thriftBinarySerialize(result, docDto);
    }
  };

  template<>
  struct BinaryDeserializer<ProcessedDocument> {
    static void deserialize(std::string &data, ProcessedDocument *result) {
      thrift_protocol::ProcessedDocumentDTO docDto;
      serialization::thriftBinaryDeserialize(data, docDto);
      result->id = docDto.id;
      result->wordVector.scores = docDto.wordVector.scores;
      result->wordVector.magnitude = docDto.wordVector.magnitude;
      result->wordVector.documentWeight = docDto.wordVector.documentWeight;
      result->updated = docDto.updated;
      result->created = docDto.created;
      if (docDto.sha1Hash.size() > 0) {
        result->sha1Hash.assign(docDto.sha1Hash);
      }
    }
  };


  template<>
  struct JsonSerializer<ProcessedDocument> {
    static std::string serialize(ProcessedDocument *document) {
      auto dynSelf = folly::toDynamic<ProcessedDocument>(*document);
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