#include <string>
#include <memory>
#include <vector>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>

#include <folly/Format.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "persistence/exceptions.h"
#include "server/ThriftRelevanceServer.h"
#include "server/RelevanceServer.h"

namespace relevanced {
namespace server {

using namespace services;
using namespace std;
using namespace folly;
using namespace persistence::exceptions;


ThriftRelevanceServer::ThriftRelevanceServer(shared_ptr<RelevanceServerIf> server)
  :server_(server) {}

void ThriftRelevanceServer::ping(){
  server_->ping();
}

Future<unique_ptr<DocumentRelevanceResponse>> ThriftRelevanceServer::future_getDocumentSimilarity(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->getDocumentSimilarity(std::move(centroidId), std::move(docId)).then([this](Try<double> result) {
    auto response = std::make_unique<DocumentRelevanceResponse>();
    if (result.hasException()) {
      if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else if (result.hasException<DocumentDoesNotExist>()) {
        response->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
      response->relevance = result.value();
    }
    return std::move(response);
  });
}

Future<unique_ptr<DocumentRelevanceResponse>> ThriftRelevanceServer::future_getTextSimilarity(unique_ptr<string> centroidId, unique_ptr<string> text) {
  return server_->getTextSimilarity(std::move(centroidId), std::move(text)).then([this](Try<double> result) {
    auto response = std::make_unique<DocumentRelevanceResponse>();
    if (result.hasException()) {
      if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else if (result.hasException<DocumentDoesNotExist>()) {
        response->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
      response->relevance = result.value();
    }
    return std::move(response);
  });
}

Future<double> ThriftRelevanceServer::future_getCentroidSimilarity(unique_ptr<string> centroid1Id, unique_ptr<string> centroid2Id) {
  return server_->getCentroidSimilarity(std::move(centroid1Id), std::move(centroid2Id)).then([this](Try<double> result) {
    if (result.hasException()) {
      return 0.0;
    }
    return result.value();
  });
}


Future<unique_ptr<services::DocumentMultiRelevanceResponse>> ThriftRelevanceServer::future_multiGetTextSimilarity(unique_ptr<vector<string>> centroidIds, unique_ptr<string> text) {
  return server_->multiGetTextSimilarity(std::move(centroidIds), std::move(text)).then([this](Try<unique_ptr<map<string, double>>> result) {
    auto response = std::make_unique<services::DocumentMultiRelevanceResponse>();
    if (result.hasException()) {
      if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
      map<string, double> scores = *result.value();
      response->scores = std::move(scores);
    }
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createDocument(unique_ptr<string> text) {
  return server_->createDocument(std::move(text)).then([this](Try<unique_ptr<string>> result) {
    auto response = std::make_unique<CrudResponse>();
    if (!result.hasException()) {
      response->status = RelevanceStatus::OK;
      response->created = *result.value();
      return std::move(response);
    }
    // can't think of any specific exceptions we're anticipating here
    response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) {
  return server_->createDocumentWithID(std::move(id), std::move(text)).then([this](Try<unique_ptr<string>> result) {
    auto response = std::make_unique<CrudResponse>();
    if (!result.hasException()) {
      response->status = RelevanceStatus::OK;
      response->created = *result.value();
      return std::move(response);
    }
    if (result.hasException<DocumentAlreadyExists>()) {
      response->status = RelevanceStatus::DOCUMENT_ALREADY_EXISTS;
    } else {
      response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
    }
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteDocument(unique_ptr<string> id) {
  return server_->deleteDocument(std::move(id)).then([this](Try<bool> result) {
    auto response = std::make_unique<CrudResponse>();
    if (!result.hasException()) {
      response->status = RelevanceStatus::OK;
      return std::move(response);
    }
    if (result.hasException<DocumentDoesNotExist>()) {
      response->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    } else {
      response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
    }
    return std::move(response);
  });
}

Future<unique_ptr<GetDocumentResponse>> ThriftRelevanceServer::future_getDocument(unique_ptr<string> id) {
  return server_->getDocument(std::move(id)).then([](Try<unique_ptr<string>> result) {
    auto response = std::make_unique<GetDocumentResponse>();
    if (result.hasException()) {
      if (result.hasException<DocumentDoesNotExist>()) {
        response->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
      response->document = *result.value();
    }
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createCentroid(unique_ptr<string> centroidId) {
  return server_->createCentroid(std::move(centroidId)).then([](Try<bool> result) {
    auto response = std::make_unique<CrudResponse>();
    if (result.hasException()) {
      if (result.hasException<CentroidAlreadyExists>()) {
        response->status = RelevanceStatus::CENTROID_ALREADY_EXISTS;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
    }
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteCentroid(unique_ptr<string> centroidId) {
  return server_->deleteCentroid(std::move(centroidId)).then([](Try<bool> result) {
    auto response = std::make_unique<CrudResponse>();
    if (result.hasException()) {
      if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
    }
    return std::move(response);
  });
}

Future<unique_ptr<ListCentroidDocumentsResponse>> ThriftRelevanceServer::future_listAllDocumentsForCentroid(unique_ptr<string> centroidId) {
  return server_->listAllDocumentsForCentroid(std::move(centroidId)).then([](Try<unique_ptr<vector<string>>> result) {
    auto response = std::make_unique<ListCentroidDocumentsResponse>();
    if (!result.hasException()) {
      response->status = RelevanceStatus::OK;
      vector<string> docIds = *result.value();
      response->documents = std::move(docIds);
    } else {
      if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    }
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_addDocumentToCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->addDocumentToCentroid(std::move(centroidId), std::move(docId)).then([](Try<bool> result) {
    auto response = std::make_unique<CrudResponse>();
    if (result.hasException()) {
      if (result.hasException<DocumentDoesNotExist>()) {
        response->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
      } else if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
    }
    return std::move(response);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_removeDocumentFromCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->removeDocumentFromCentroid(std::move(centroidId), std::move(docId)).then([](Try<bool> result) {
    auto response = std::make_unique<CrudResponse>();
    if (result.hasException()) {
      if (result.hasException<DocumentDoesNotExist>()) {
        response->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
      } else if (result.hasException<CentroidDoesNotExist>()) {
        response->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
      } else {
        response->status = RelevanceStatus::UNKNOWN_EXCEPTION;
      }
    } else {
      response->status = RelevanceStatus::OK;
    }
    return std::move(response);
  });
}

Future<bool> ThriftRelevanceServer::future_recomputeCentroid(unique_ptr<string> centroidId) {
  return server_->recomputeCentroid(std::move(centroidId)).then([](Try<bool> result) {
    if (result.hasException()) {
      return false;
    }
    return true;
  });
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listAllCentroids() {
  return server_->listAllCentroids();
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listAllDocuments() {
  return server_->listAllDocuments();
}

} // server
} // relevanced
