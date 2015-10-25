#include <string>
#include <memory>
#include <vector>
#include <map>


#include <folly/futures/Future.h>
#include <folly/futures/Try.h>

#include <folly/Format.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevanced.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "server/ThriftRelevanceServer.h"
#include "server/RelevanceServer.h"
#include "models/Centroid.h"
#include "models/ProcessedDocument.h"

namespace relevanced {
namespace server {

using namespace thrift_protocol;
using models::Centroid;
using models::ProcessedDocument;
using namespace std;
using namespace folly;


ThriftRelevanceServer::ThriftRelevanceServer(
    shared_ptr<RelevanceServerIf> server)
    : server_(server) {}

void ThriftRelevanceServer::ping() { server_->ping(); }

Future<double> ThriftRelevanceServer::future_getDocumentSimilarity(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->getDocumentSimilarity(std::move(centroidId), std::move(docId))
      .then([this](Try<double> result) {
        result.throwIfFailed();
        return result.value();
      });
}

Future<double> ThriftRelevanceServer::future_getTextSimilarity(
    unique_ptr<string> centroidId, unique_ptr<string> text) {
  return server_->getTextSimilarity(std::move(centroidId), std::move(text))
      .then([this](Try<double> result) {
        result.throwIfFailed();
        return result.value();
      });
}

Future<double> ThriftRelevanceServer::future_getCentroidSimilarity(
    unique_ptr<string> centroid1Id, unique_ptr<string> centroid2Id) {
  return server_->getCentroidSimilarity(std::move(centroid1Id),
                                        std::move(centroid2Id))
      .then([this](Try<double> result) {
        result.throwIfFailed();
        return result.value();
      });
}


Future<unique_ptr<MultiSimilarityResponse>>
ThriftRelevanceServer::future_multiGetTextSimilarity(
    unique_ptr<vector<string>> centroidIds, unique_ptr<string> text) {
  return server_->multiGetTextSimilarity(std::move(centroidIds),
                                         std::move(text))
      .then([this](Try<unique_ptr<map<string, double>>> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<MultiSimilarityResponse>();
        map<string, double> scores = *result.value();
        response->scores = std::move(scores);
        return std::move(response);
      });
}

Future<unique_ptr<MultiSimilarityResponse>>
ThriftRelevanceServer::future_multiGetDocumentSimilarity(
    unique_ptr<vector<string>> centroidIds, unique_ptr<string> docId) {
  return server_->multiGetDocumentSimilarity(std::move(centroidIds),
                                         std::move(docId))
      .then([this](Try<unique_ptr<map<string, double>>> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<MultiSimilarityResponse>();
        map<string, double> scores = *result.value();
        response->scores = std::move(scores);
        return std::move(response);
      });
}


Future<unique_ptr<CreateDocumentResponse>>
ThriftRelevanceServer::future_createDocument(unique_ptr<string> text) {
  return server_->createDocument(std::move(text))
      .then([this](Try<unique_ptr<string>> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<CreateDocumentResponse>();
        response->id = *result.value();
        return std::move(response);
      });
}

Future<unique_ptr<CreateDocumentResponse>>
ThriftRelevanceServer::future_createDocumentWithID(unique_ptr<string> id,
                                                   unique_ptr<string> text) {
  string docId = *id;
  return server_->createDocumentWithID(std::move(id), std::move(text))
      .then([this, docId](Try<unique_ptr<string>> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<CreateDocumentResponse>();
        response->id = docId;
        return std::move(response);
      });
}

Future<unique_ptr<DeleteDocumentResponse>>
ThriftRelevanceServer::future_deleteDocument(unique_ptr<string> id) {
  string docId = *id;
  return server_->deleteDocument(std::move(id))
      .then([this, docId](Try<bool> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<DeleteDocumentResponse>();
        response->id = docId;
        return std::move(response);
      });
}


Future<unique_ptr<CreateCentroidResponse>>
ThriftRelevanceServer::future_createCentroid(unique_ptr<string> centroidId) {
  string cId = *centroidId;
  return server_->createCentroid(std::move(centroidId))
      .then([cId](Try<bool> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<CreateCentroidResponse>();
        response->id = cId;
        return std::move(response);
      });
}

Future<unique_ptr<DeleteCentroidResponse>>
ThriftRelevanceServer::future_deleteCentroid(unique_ptr<string> centroidId) {
  string cId = *centroidId;
  return server_->deleteCentroid(std::move(centroidId))
      .then([cId](Try<bool> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<DeleteCentroidResponse>();
        response->id = cId;
        return std::move(response);
      });
}

Future<unique_ptr<ListCentroidDocumentsResponse>>
ThriftRelevanceServer::future_listAllDocumentsForCentroid(
    unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  return server_->listAllDocumentsForCentroid(std::move(centroidId))
      .then([cId](Try<unique_ptr<vector<string>>> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<ListCentroidDocumentsResponse>();
        vector<string> docIds = *result.value();
        response->documents = std::move(docIds);
        return std::move(response);
      });
}

Future<unique_ptr<AddDocumentToCentroidResponse>>
ThriftRelevanceServer::future_addDocumentToCentroid(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  auto dId = *docId;
  return server_->addDocumentToCentroid(std::move(centroidId), std::move(docId))
      .then([cId, dId](Try<bool> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<AddDocumentToCentroidResponse>();
        response->documentId = dId;
        response->centroidId = cId;
        return std::move(response);
      });
}

Future<unique_ptr<RemoveDocumentFromCentroidResponse>>
ThriftRelevanceServer::future_removeDocumentFromCentroid(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto cId = *centroidId;
  auto dId = *docId;
  return server_->removeDocumentFromCentroid(std::move(centroidId),
                                             std::move(docId))
      .then([cId, dId](Try<bool> result) {
        result.throwIfFailed();
        auto response =
            folly::make_unique<RemoveDocumentFromCentroidResponse>();
        response->documentId = dId;
        response->centroidId = cId;
        return std::move(response);
      });
}

Future<unique_ptr<JoinCentroidResponse>>
ThriftRelevanceServer::future_joinCentroid(unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  return server_->joinCentroid(std::move(centroidId))
      .then([cId](Try<bool> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<JoinCentroidResponse>();
        response->id = cId;
        response->recalculated = result.value();
        return std::move(response);
      });
}

Future<unique_ptr<ListCentroidsResponse>>
ThriftRelevanceServer::future_listAllCentroids() {
  return server_->listAllCentroids().then(
      [](unique_ptr<vector<string>> result) {
        auto response = folly::make_unique<ListCentroidsResponse>();
        response->centroids = *result;
        return std::move(response);
      });
}

Future<unique_ptr<ListDocumentsResponse>>
ThriftRelevanceServer::future_listAllDocuments() {
  return server_->listAllDocuments().then(
      [](unique_ptr<vector<string>> result) {
        auto response = folly::make_unique<ListDocumentsResponse>();
        response->documents = *result;
        return std::move(response);
      });
}

Future<unique_ptr<map<string, string>>>
ThriftRelevanceServer::future_getServerMetadata() {
  return server_->getServerMetadata();
}

Future<folly::Unit>
ThriftRelevanceServer::future_debugEraseAllData() {
  return server_->debugEraseAllData();
}

Future<unique_ptr<CentroidDTO>>
ThriftRelevanceServer::future_debugGetFullCentroid(unique_ptr<string> centroidId) {
  return server_->debugGetFullCentroid(std::move(centroidId)).then([](Try<shared_ptr<Centroid>> result) {
    result.throwIfFailed();
    auto centroid = result.value();
    auto response = folly::make_unique<CentroidDTO>();
    response->id = centroid->id;
    response->wordVector.magnitude = centroid->wordVector.magnitude;
    response->wordVector.documentWeight = centroid->wordVector.documentWeight;
    response->wordVector.scores = centroid->wordVector.scores;
    return std::move(response);
  });
}

Future<unique_ptr<ProcessedDocumentDTO>>
ThriftRelevanceServer::future_debugGetFullProcessedDocument(unique_ptr<string> documentId) {
  return server_->debugGetFullProcessedDocument(std::move(documentId)).then([](Try<shared_ptr<ProcessedDocument>> result) {
    result.throwIfFailed();
    auto document = result.value();
    auto response = folly::make_unique<ProcessedDocumentDTO>();
    response->metadata.id = document->id;
    response->metadata.created = document->created;
    response->metadata.updated = document->updated;
    if (document->sha1Hash.hasValue()) {
      response->metadata.sha1Hash = document->sha1Hash.value();
    }
    response->wordVector.magnitude = document->magnitude;
    for (auto &elem: document->scoredWords) {
      string k = elem.word;
      response->wordVector.scores[k] = elem.score;
    }
    // response->wordVector.documentWeight = document->wordVector.documentWeight;
    // response->wordVector.scores = document->wordVector.scores;
    return std::move(response);
  });
}


} // server
} // relevanced
