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
using thrift_protocol::Language;

ThriftRelevanceServer::ThriftRelevanceServer(
    shared_ptr<RelevanceServerIf> server)
    : server_(server) {}

void ThriftRelevanceServer::ping() { server_->ping(); }

Future<double> ThriftRelevanceServer::future_getDocumentSimilarity(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->getDocumentSimilarity(
    std::move(centroidId), std::move(docId)
  ).then([this](Try<double> result) {
    result.throwIfFailed();
    return result.value();
  });
}

Future<double> ThriftRelevanceServer::future_getTextSimilarity(
    unique_ptr<string> centroidId,
    unique_ptr<string> text,
    Language lang) {
  return server_->getTextSimilarity(
    std::move(centroidId), std::move(text), lang
  ).then([this](Try<double> result) {
    result.throwIfFailed();
    return result.value();
  });
}

Future<double> ThriftRelevanceServer::future_getCentroidSimilarity(
    unique_ptr<string> centroid1Id, unique_ptr<string> centroid2Id) {
  return server_->getCentroidSimilarity(
    std::move(centroid1Id),
    std::move(centroid2Id)
  ).then([this](Try<double> result) {
    result.throwIfFailed();
    return result.value();
  });
}


Future<unique_ptr<MultiSimilarityResponse>>
ThriftRelevanceServer::future_multiGetTextSimilarity(
    unique_ptr<vector<string>> centroidIds,
    unique_ptr<string> text,
    Language lang) {
  return server_->multiGetTextSimilarity(
    std::move(centroidIds), std::move(text), lang
  ).then([this](Try<unique_ptr<map<string, double>>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<MultiSimilarityResponse>();
    map<string, double> scores = *result.value();
    response->scores = std::move(scores);
    return std::move(response);
  });
}

Future<unique_ptr<MultiSimilarityResponse>>
ThriftRelevanceServer::future_multiGetDocumentSimilarity(
    unique_ptr<vector<string>> centroidIds,
    unique_ptr<string> docId) {
  return server_->multiGetDocumentSimilarity(
    std::move(centroidIds),
    std::move(docId)
  ).then([this](Try<unique_ptr<map<string, double>>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<MultiSimilarityResponse>();
    map<string, double> scores = *result.value();
    response->scores = std::move(scores);
    return std::move(response);
  });
}


Future<unique_ptr<CreateDocumentResponse>>
ThriftRelevanceServer::future_createDocument(
    unique_ptr<string> text, Language lang) {
  return server_->createDocument(
    std::move(text), lang
  ).then([this](Try<unique_ptr<string>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<CreateDocumentResponse>();
    response->id = *result.value();
    return std::move(response);
  });
}

Future<unique_ptr<CreateDocumentResponse>>
ThriftRelevanceServer::future_createDocumentWithID(
    unique_ptr<string> id,
    unique_ptr<string> text,
    Language lang) {
  string docId = *id;
  return server_->createDocumentWithID(
    std::move(id), std::move(text), lang
  ).then([this, docId](Try<unique_ptr<string>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<CreateDocumentResponse>();
    response->id = docId;
    return std::move(response);
  });
}

Future<unique_ptr<DeleteDocumentResponse>>
ThriftRelevanceServer::future_deleteDocument(
    unique_ptr<DeleteDocumentRequest> request) {
  string docId = request->id;
  return server_->deleteDocument(
    folly::make_unique<string>(request->id),
    request->ignoreMissing
  ).then([this, docId](Try<bool> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<DeleteDocumentResponse>();
    response->id = docId;
    return std::move(response);
  });
}

Future<unique_ptr<MultiDeleteDocumentsResponse>>
ThriftRelevanceServer::future_multiDeleteDocuments(
    unique_ptr<MultiDeleteDocumentsRequest> request) {
  auto ids = std::make_shared<vector<string>>(request->ids);
  bool ignoreMissing = request->ignoreMissing;
  return server_->multiDeleteDocuments(
    folly::make_unique<vector<string>>(request->ids),
    request->ignoreMissing
  ).then([ids, ignoreMissing](vector<Try<bool>> result) {
    vector<string> deleted;
    for (size_t i = 0; i < result.size(); i++) {
      auto elem = result.at(i);
      if (elem.hasException()) {
        if (!ignoreMissing || !elem.hasException<EDocumentDoesNotExist>()) {
          elem.throwIfFailed();
        }
      }
      if (elem.value()) {
        deleted.push_back(ids->at(i));
      }
    }
    auto response = folly::make_unique<MultiDeleteDocumentsResponse>();
    response->ids = *ids;
    return std::move(response);
  });
}

Future<unique_ptr<CreateCentroidResponse>>
ThriftRelevanceServer::future_createCentroid(
    unique_ptr<CreateCentroidRequest> request) {
  auto cId = request->id;
  return server_->createCentroid(
    folly::make_unique<string>(request->id),
    request->ignoreExisting
  ).then([cId](Try<bool> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<CreateCentroidResponse>();
    if (result.value()) {
      response->created = cId;
    } else {
      response->created = "";
    }
    return std::move(response);
  });
}

Future<unique_ptr<MultiCreateCentroidsResponse>>
ThriftRelevanceServer::future_multiCreateCentroids(
    unique_ptr<MultiCreateCentroidsRequest> request) {
  auto ids = std::make_shared<vector<string>>(request->ids);
  return server_->multiCreateCentroids(
    folly::make_unique<vector<string>>(request->ids),
    request->ignoreExisting
  ).then([ids](vector<Try<bool>> result) {
    vector<string> created;
    for (size_t i = 0; i < result.size(); i++) {
      auto elem = result.at(i);
      elem.throwIfFailed();
      if (elem.value()) {
        created.push_back(ids->at(i));
      }
    }
    auto response = folly::make_unique<MultiCreateCentroidsResponse>();
    response->created = created;
    return std::move(response);
  });
}

Future<unique_ptr<DeleteCentroidResponse>>
ThriftRelevanceServer::future_deleteCentroid(
    unique_ptr<DeleteCentroidRequest> request) {
  string cId = request->id;
  bool ignoreMissing = request->ignoreMissing;
  return server_->deleteCentroid(
    folly::make_unique<string>(request->id),
    request->ignoreMissing
  ).then([cId, ignoreMissing](Try<bool> result) {
    if (result.hasException()) {
      if (!ignoreMissing || !result.hasException<ECentroidDoesNotExist>()) {
        result.throwIfFailed();
      }
    }
    auto response = folly::make_unique<DeleteCentroidResponse>();
    response->id = cId;
    return std::move(response);
  });
}

Future<unique_ptr<MultiDeleteCentroidsResponse>>
ThriftRelevanceServer::future_multiDeleteCentroids(
    unique_ptr<MultiDeleteCentroidsRequest> request) {
  auto ids = std::make_shared<vector<string>>(request->ids);
  bool ignoreMissing = request->ignoreMissing;
  return server_->multiDeleteCentroids(
    folly::make_unique<vector<string>>(request->ids),
    request->ignoreMissing
  ).then([ids, ignoreMissing](vector<Try<bool>> result) {
    vector<string> deleted;
    for (size_t i = 0; i < result.size(); i++) {
      auto elem = result.at(i);
      if (elem.hasException()) {
        if (!ignoreMissing || !elem.hasException<ECentroidDoesNotExist>()) {
          elem.throwIfFailed();
        }
      }
      if (elem.value()) {
        deleted.push_back(ids->at(i));
      }
    }
    auto response = folly::make_unique<MultiDeleteCentroidsResponse>();
    response->ids = *ids;
    return std::move(response);
  });
}


Future<unique_ptr<ListCentroidDocumentsResponse>>
ThriftRelevanceServer::future_listAllDocumentsForCentroid(
    unique_ptr<string> centroidId) {
  auto cId = *centroidId;
  return server_->listAllDocumentsForCentroid(
    std::move(centroidId)
  ).then([cId](Try<unique_ptr<vector<string>>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<ListCentroidDocumentsResponse>();
    vector<string> docIds = *result.value();
    response->documents = std::move(docIds);
    return std::move(response);
  });
}

Future<unique_ptr<ListCentroidDocumentsResponse>>
ThriftRelevanceServer::future_listCentroidDocumentRange(
    unique_ptr<string> centroidId,
    int64_t iOffset,
    int64_t iCount) {

  size_t offset = iOffset;
  size_t count = iCount;
  return server_->listCentroidDocumentRange(
    std::move(centroidId), offset, count
  ).then([](Try<unique_ptr<vector<string>>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<ListCentroidDocumentsResponse>();
    vector<string> docIds = *result.value();
    response->documents = std::move(docIds);
    return std::move(response);
  });
}

Future<unique_ptr<ListCentroidDocumentsResponse>>
ThriftRelevanceServer::future_listCentroidDocumentRangeFromID(
    unique_ptr<string> centroidId,
    unique_ptr<string> docId,
    int64_t iCount) {

  size_t count = iCount;
  return server_->listCentroidDocumentRangeFromID(
    std::move(centroidId), std::move(docId), count
  ).then([](Try<unique_ptr<vector<string>>> result) {
    result.throwIfFailed();
    auto response = folly::make_unique<ListCentroidDocumentsResponse>();
    vector<string> docIds = *result.value();
    response->documents = std::move(docIds);
    return std::move(response);
  });
}

Future<unique_ptr<AddDocumentsToCentroidResponse>>
ThriftRelevanceServer::future_addDocumentsToCentroid(
    unique_ptr<AddDocumentsToCentroidRequest> request) {
  return server_->addDocumentsToCentroid(
    std::move(request)
  ).then([](Try<unique_ptr<AddDocumentsToCentroidResponse>> result) {
    result.throwIfFailed();
    return std::move(result.value());
  });
}

Future<unique_ptr<RemoveDocumentsFromCentroidResponse>>
ThriftRelevanceServer::future_removeDocumentsFromCentroid(
    unique_ptr<RemoveDocumentsFromCentroidRequest> request) {
  return server_->removeDocumentsFromCentroid(
    std::move(request)
  ).then([](Try<unique_ptr<RemoveDocumentsFromCentroidResponse>> result) {
    result.throwIfFailed();
    return std::move(result.value());
  });
}

Future<unique_ptr<JoinCentroidResponse>>
ThriftRelevanceServer::future_joinCentroid(
    unique_ptr<JoinCentroidRequest> request) {
  auto cId = request->id;
  bool ignoreMissing = request->ignoreMissing;
  return server_->joinCentroid(
    folly::make_unique<string>(request->id),
    request->ignoreMissing
  ).then([cId, ignoreMissing](Try<bool> result) {
    if (result.hasException()) {
      if (!ignoreMissing || !result.hasException<ECentroidDoesNotExist>()) {
        result.throwIfFailed();
      }
    }
    auto response = folly::make_unique<JoinCentroidResponse>();
    response->id = cId;
    response->recalculated = result.value();
    return std::move(response);
  });
}

Future<unique_ptr<MultiJoinCentroidsResponse>>
ThriftRelevanceServer::future_multiJoinCentroids(
    unique_ptr<MultiJoinCentroidsRequest> request) {
  auto cIds = std::make_shared<vector<string>>(request->ids);
  bool ignoreMissing = request->ignoreMissing;
  return server_->multiJoinCentroids(
    folly::make_unique<vector<string>>(request->ids),
    request->ignoreMissing
  ).then([cIds, ignoreMissing](unique_ptr<vector<Try<bool>>> result) {
    vector<bool> recalculations;
    for (auto &elem: *result) {
      if (elem.hasException()) {
        if (!ignoreMissing || !elem.hasException<ECentroidDoesNotExist>()) {
          elem.throwIfFailed();
        }
      }
      recalculations.push_back(elem.value());
    }
    auto response = folly::make_unique<MultiJoinCentroidsResponse>();
    response->ids = *cIds;
    response->recalculated = recalculations;
    return std::move(response);
  });
}


Future<unique_ptr<ListCentroidsResponse>>
ThriftRelevanceServer::future_listAllCentroids() {
  return server_->listAllCentroids()
    .then([](unique_ptr<vector<string>> result) {
      auto response = folly::make_unique<ListCentroidsResponse>();
      response->centroids = *result;
      return std::move(response);
    });
}

Future<unique_ptr<ListCentroidsResponse>>
ThriftRelevanceServer::future_listCentroidRange(int64_t iOffset, int64_t iCount) {
  size_t offset = iOffset;
  size_t count = iCount;
  return server_->listCentroidRange(offset, count).then(
      [](unique_ptr<vector<string>> result) {
        auto response = folly::make_unique<ListCentroidsResponse>();
        response->centroids = *result;
        return std::move(response);
      });
}

Future<unique_ptr<ListCentroidsResponse>>
ThriftRelevanceServer::future_listCentroidRangeFromID(unique_ptr<string> centroidId, int64_t iCount) {
  size_t count = iCount;
  return server_->listCentroidRangeFromID(std::move(centroidId), count).then(
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

Future<unique_ptr<ListDocumentsResponse>>
ThriftRelevanceServer::future_listUnusedDocuments(int64_t count) {
  return server_->listUnusedDocuments((size_t) count).then(
    [](unique_ptr<vector<string>> result) {
      auto response = folly::make_unique<ListDocumentsResponse>();
      response->documents = *result;
      return std::move(response);
    });
}

Future<unique_ptr<ListDocumentsResponse>>
ThriftRelevanceServer::future_listDocumentRange(int64_t iOffset, int64_t iCount) {
  size_t offset = iOffset;
  size_t count = iCount;
  return server_->listDocumentRange(offset, count).then(
      [](unique_ptr<vector<string>> result) {
        auto response = folly::make_unique<ListDocumentsResponse>();
        response->documents = *result;
        return std::move(response);
      });
}

Future<unique_ptr<ListDocumentsResponse>>
ThriftRelevanceServer::future_listDocumentRangeFromID(unique_ptr<string> docId, int64_t iCount) {
  size_t count = iCount;
  return server_->listDocumentRangeFromID(std::move(docId), count).then(
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
    return std::move(response);
  });
}


} // server
} // relevanced
