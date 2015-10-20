#include <string>
#include <map>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include <folly/json.h>
#include "models/ProcessedDocument.h"
#include "models/WordVector.h"
#include "serialization/serializers.h"
#include "serialization/serializer_details.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "text_util/ScoredWord.h"
namespace folly {

using namespace std;
using relevanced::text_util::ScoredWord;
using relevanced::models::ProcessedDocument;
using relevanced::models::WordVector;

template <>
struct DynamicConstructor<ProcessedDocument> {
  static folly::dynamic construct(const ProcessedDocument &doc) {
    WordVector wordVec;
    for (auto &elem: doc.scoredWords) {
      string k = elem.word;
      wordVec.scores.insert(make_pair(k, elem.score));
    }
    wordVec.magnitude = doc.magnitude;
    auto dWordVec = folly::toDynamic(wordVec);
    folly::dynamic self = folly::dynamic::object;
    self["id"] = doc.id;
    self["wordVector"] = dWordVec;
    self["created"] = doc.created;
    self["updated"] = doc.updated;
    if (doc.sha1Hash.hasValue()) {
      self["sha1Hash"] = doc.sha1Hash.value();
    } else {
      self["sha1Hash"] = "";
    }
    return self;
  }
};

template <>
struct DynamicConverter<ProcessedDocument> {
  static ProcessedDocument convert(const folly::dynamic &dyn) {
    auto id = folly::convertTo<std::string>(dyn["id"]);
    ProcessedDocument result(id);
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
using text_util::ScoredWord;

template <>
struct BinarySerializer<ProcessedDocument> {
  static void serialize(std::string &result, ProcessedDocument &target) {
    thrift_protocol::ProcessedDocumentPersistenceDTO docDto;
    thrift_protocol::ProcessedDocumentMetadataDTO metadataDto;

    for (auto &elem: target.scoredWords) {
      thrift_protocol::ScoredWordDTO wordDto;
      wordDto.wordBuff = elem.word;
      wordDto.score = elem.score;
      docDto.scoredWords.push_back(wordDto);
    }
    docDto.magnitude = target.magnitude;

    metadataDto.id = target.id;
    metadataDto.updated = target.updated;
    metadataDto.created = target.created;
    if (target.sha1Hash.hasValue()) {
      metadataDto.sha1Hash = target.sha1Hash.value();
    } else {
      metadataDto.sha1Hash = "";
    }
    docDto.metadata = metadataDto;
    serialization::thriftBinarySerialize(result, docDto);
  }
};

template <>
struct BinaryDeserializer<ProcessedDocument> {
  static void deserialize(std::string &data, ProcessedDocument *result) {
    thrift_protocol::ProcessedDocumentPersistenceDTO docDto;
    serialization::thriftBinaryDeserialize(data, docDto);
    for (auto &elem: docDto.scoredWords) {
      uint8_t buffSize = (uint8_t) elem.wordBuff.size();
      result->scoredWords.push_back(ScoredWord{elem.wordBuff.c_str(), buffSize, elem.score});
    }
    result->magnitude = docDto.magnitude;
    result->updated = docDto.metadata.updated;
    result->created = docDto.metadata.created;
    result->id = docDto.metadata.id;
    if (docDto.metadata.sha1Hash.size() > 0) {
      result->sha1Hash.assign(docDto.metadata.sha1Hash);
    }
  }
};


template <>
struct JsonSerializer<ProcessedDocument> {
  static void serialize(std::string &result, ProcessedDocument *document) {
    auto dynSelf = folly::toDynamic<ProcessedDocument>(*document);
    folly::fbstring js = folly::toJson(dynSelf);
    result = js.toStdString();
  }
};

template <>
struct JsonDeserializer<ProcessedDocument> {
  static void deserialize(const std::string &js, ProcessedDocument *result) {
    auto dynSelf = folly::parseJson(js);
    *result = folly::convertTo<ProcessedDocument>(dynSelf);
  }
};

} // serialization
} // relevanced
