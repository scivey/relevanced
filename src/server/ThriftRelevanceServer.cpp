#include <string>
#include <memory>
#include <vector>
#include <map>


#include <folly/futures/Future.h>
#include <folly/futures/Try.h>

#include <folly/Format.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevanced.h"
#include "persistence/exceptions.h"
#include "server/ThriftRelevanceServer.h"
#include "server/RelevanceServer.h"

namespace relevanced {
namespace server {

using namespace thrift_protocol;
using namespace std;
using namespace folly;
using namespace persistence::exceptions;


ThriftRelevanceServer::ThriftRelevanceServer(
    shared_ptr<RelevanceServerIf> server)
    : server_(server) {}

void ThriftRelevanceServer::ping() { server_->ping(); }

Future<unique_ptr<SimilarityResponse>>
ThriftRelevanceServer::future_getDocumentSimilarity(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->getDocumentSimilarity(std::move(centroidId), std::move(docId))
      .then([this](Try<double> result) {
        auto response = folly::make_unique<SimilarityResponse>();
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            response->status.code = StatusCode::CENTROID_DOES_NOT_EXIST;
          } else if (result.hasException<DocumentDoesNotExist>()) {
            response->status.code = StatusCode::DOCUMENT_DOES_NOT_EXIST;
          } else {
            response->status.code = StatusCode::UNKNOWN_EXCEPTION;
          }
        } else {
          response->status.code = StatusCode::OK;
          response->similarity = result.value();
        }
        return std::move(response);
      });
}

Future<unique_ptr<SimilarityResponse>>
ThriftRelevanceServer::future_getTextSimilarity(unique_ptr<string> centroidId,
                                                unique_ptr<string> text) {
  return server_->getTextSimilarity(std::move(centroidId), std::move(text))
      .then([this](Try<double> result) {
        auto response = folly::make_unique<SimilarityResponse>();
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            response->status.code = StatusCode::CENTROID_DOES_NOT_EXIST;
          } else if (result.hasException<DocumentDoesNotExist>()) {
            response->status.code = StatusCode::DOCUMENT_DOES_NOT_EXIST;
          } else {
            response->status.code = StatusCode::UNKNOWN_EXCEPTION;
          }
        } else {
          response->status.code = StatusCode::OK;
          response->similarity = result.value();
        }
        return std::move(response);
      });
}

Future<unique_ptr<SimilarityResponse>>
ThriftRelevanceServer::future_getCentroidSimilarity(
    unique_ptr<string> centroid1Id, unique_ptr<string> centroid2Id) {
  return server_->getCentroidSimilarity(std::move(centroid1Id),
                                        std::move(centroid2Id))
      .then([this](Try<double> result) {
        auto response = folly::make_unique<SimilarityResponse>();
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            response->status.code = StatusCode::CENTROID_DOES_NOT_EXIST;
          } else if (result.hasException<DocumentDoesNotExist>()) {
            response->status.code = StatusCode::DOCUMENT_DOES_NOT_EXIST;
          } else {
            response->status.code = StatusCode::UNKNOWN_EXCEPTION;
          }
        } else {
          response->status.code = StatusCode::OK;
          response->similarity = result.value();
        }
        return std::move(response);
      });
}


Future<unique_ptr<MultiSimilarityResponse>>
ThriftRelevanceServer::future_multiGetTextSimilarity(
    unique_ptr<vector<string>> centroidIds, unique_ptr<string> text) {
  return server_->multiGetTextSimilarity(std::move(centroidIds),
                                         std::move(text))
      .then([this](Try<unique_ptr<map<string, double>>> result) {
        auto response = folly::make_unique<MultiSimilarityResponse>();
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            response->status.code = StatusCode::CENTROID_DOES_NOT_EXIST;
          } else {
            response->status.code = StatusCode::UNKNOWN_EXCEPTION;
          }
        } else {
          response->status.code = StatusCode::OK;
          map<string, double> scores = *result.value();
          response->scores = std::move(scores);
        }
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createDocument(
    unique_ptr<string> text) {
  return server_->createDocument(std::move(text))
      .then([this](Try<unique_ptr<string>> result) {
        result.throwIfFailed();
        auto response = folly::make_unique<CrudResponse>();
        response->id = *result.value();
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>>
ThriftRelevanceServer::future_createDocumentWithID(unique_ptr<string> id,
                                                   unique_ptr<string> text) {
  string docId = *id;
  return server_->createDocumentWithID(std::move(id), std::move(text))
      .then([this, docId](Try<unique_ptr<string>> result) {
        if (result.hasException()) {
          if (result.hasException<DocumentAlreadyExists>()) {
            TDocumentAlreadyExists exn;
            exn.id = docId;
            throw exn;
          }
          result.throwIfFailed();
        }
        auto response = folly::make_unique<CrudResponse>();
        response->id = docId;
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteDocument(
    unique_ptr<string> id) {
  string docId = *id;
  return server_->deleteDocument(std::move(id))
      .then([this, docId](Try<bool> result) {
        if (result.hasException()) {
          if (result.hasException<DocumentDoesNotExist>()) {
            TDocumentDoesNotExist exn;
            exn.id = docId;
            throw exn;
          }
          result.throwIfFailed();
        }
        auto response = folly::make_unique<CrudResponse>();
        response->id = docId;
        return std::move(response);
      });
}

Future<unique_ptr<GetDocumentResponse>>
ThriftRelevanceServer::future_getDocument(unique_ptr<string> id) {
  auto docId = *id;
  return server_->getDocument(std::move(id))
      .then([docId](Try<unique_ptr<string>> result) {
        auto response = folly::make_unique<GetDocumentResponse>();
        if (result.hasException()) {
          if (result.hasException<DocumentDoesNotExist>()) {
            TDocumentDoesNotExist exn;
            exn.id = docId;
            throw exn;
          }
          result.throwIfFailed();
        }
        response->status.code = StatusCode::OK;
        response->document = *result.value();
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createCentroid(
    unique_ptr<string> centroidId) {
  string cId = *centroidId;
  return server_->createCentroid(std::move(centroidId))
      .then([cId](Try<bool> result) {
        if (result.hasException()) {
          if (result.hasException<CentroidAlreadyExists>()) {
            TCentroidAlreadyExists exn;
            exn.id = cId;
            throw exn;
          } else {
            result.throwIfFailed();
          }
        }
        auto response = folly::make_unique<CrudResponse>();
        response->id = cId;
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteCentroid(
    unique_ptr<string> centroidId) {
  string cId = *centroidId;
  return server_->deleteCentroid(std::move(centroidId))
      .then([cId](Try<bool> result) {
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            TCentroidDoesNotExist exn;
            exn.id = cId;
            throw exn;
          } else {
            result.throwIfFailed();
          }
        }
        auto response = folly::make_unique<CrudResponse>();
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
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            TCentroidDoesNotExist exn;
            exn.id = cId;
            throw exn;
          }
          result.throwIfFailed();
        }
        auto response = folly::make_unique<ListCentroidDocumentsResponse>();
        vector<string> docIds = *result.value();
        response->documents = std::move(docIds);
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>>
ThriftRelevanceServer::future_addDocumentToCentroid(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->addDocumentToCentroid(std::move(centroidId), std::move(docId))
      .then([](Try<bool> result) {
        if (result.hasException()) {
          if (result.hasException<DocumentDoesNotExist>()) {
            TDocumentDoesNotExist exn;
            throw exn;
          } else if (result.hasException<CentroidDoesNotExist>()) {
            TCentroidDoesNotExist exn;
            throw exn;
          } else {
            result.throwIfFailed();
          }
        }
        auto response = folly::make_unique<CrudResponse>();
        return std::move(response);
      });
}

Future<unique_ptr<CrudResponse>>
ThriftRelevanceServer::future_removeDocumentFromCentroid(
    unique_ptr<string> centroidId, unique_ptr<string> docId) {
  auto dId = *docId;
  return server_->removeDocumentFromCentroid(std::move(centroidId),
                                             std::move(docId))
      .then([dId](Try<bool> result) {
        if (result.hasException()) {
          if (result.hasException<DocumentDoesNotExist>()) {
            TDocumentDoesNotExist exn;
            throw exn;
          } else if (result.hasException<CentroidDoesNotExist>()) {
            TCentroidDoesNotExist exn;
            throw exn;
          }
          result.throwIfFailed();
        }
        auto response = folly::make_unique<CrudResponse>();
        response->id = dId;
        return std::move(response);
      });
}

Future<bool> ThriftRelevanceServer::future_joinCentroid(
    unique_ptr<string> centroidId) {
  return server_->joinCentroid(std::move(centroidId))
      .then([](Try<bool> result) {
        if (result.hasException()) {
          if (result.hasException<CentroidDoesNotExist>()) {
            TCentroidDoesNotExist exn;
            throw exn;
          }
          result.throwIfFailed();
        }
        return result.value();
      });
}

Future<unique_ptr<vector<string>>>
ThriftRelevanceServer::future_listAllCentroids() {
  return server_->listAllCentroids();
}

Future<unique_ptr<vector<string>>>
ThriftRelevanceServer::future_listAllDocuments() {
  return server_->listAllDocuments();
}

Future<unique_ptr<map<string, string>>>
ThriftRelevanceServer::future_getServerMetadata() {
  return server_->getServerMetadata();
}

} // server
} // relevanced
