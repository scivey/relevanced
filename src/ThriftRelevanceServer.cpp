#include <string>
#include <memory>
#include <vector>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>

#include <folly/Format.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "persistence/exceptions.h"
#include "ThriftRelevanceServer.h"
#include "RelevanceServer.h"

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
  return server_->getDocumentSimilarity(std::move(centroidId), std::move(docId)).then([this](Try<double> relevance) {
    auto res = std::make_unique<DocumentRelevanceResponse>();
    if (!relevance.hasException()) {
      res->status = RelevanceStatus::OK;
      res->relevance = relevance.value();
      return std::move(res);
    }
    res->relevance = 0.0;
    if (relevance.hasException<CentroidDoesNotExist>()) {
      res->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
    } else if (relevance->hasException<DocumentDoesNotExist>()) {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    } else {
      res->status = RelevanceStatus::UNKNOWN_EXCEPTION;
    }
    return std::move(res);
  });
}

Future<unique_ptr<DocumentRelevanceResponse>> ThriftRelevanceServer::future_getTextSimilarity(unique_ptr<string> centroidId, unique_ptr<string> text) {
  return server_->getTextSimilarity(std::move(centroidId), std::move(text)).then([this](Try<double> relevance) {
    auto res = std::make_unique<DocumentRelevanceResponse>();
    if (!relevance.hasException()) {
      res->status = RelevanceStatus::OK;
      res->relevance = relevance.value();
      return std::move(res);
    }
    res->relevance = 0.0;
    if (relevance.hasException<CentroidDoesNotExist>()) {
      res->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
    } else if (relevance->hasException<DocumentDoesNotExist>()) {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    } else {
      res->status = RelevanceStatus::UNKNOWN_EXCEPTION;
    }
    return std::move(res);
  });
}

Future<unique_ptr<services::DocumentMultiRelevanceResponse>> ThriftRelevanceServer::future_multiGetTextSimilarity(unique_ptr<vector<string>> centroidIds, unique_ptr<string> text) {
  return server_->multiGetTextSimilarity(std::move(centroidIds), std::move(text)).then([this](Try<unique_ptr<map<string, double>>> scores) {
    auto res = std::make_unique<services::DocumentMultiRelevanceResponse>();
    if (!scores.hasException()) {
      res->status = RelevanceStatus::OK;
      res->scores = *scores;
      return std::move(res);
    }
    if (scores.hasException<CentroidDoesNotExist>()) {
      res->status = RelevanceStatus::CENTROID_DOES_NOT_EXIST;
    } else {
      res->status = RelevanceStatus::UNKNOWN_EXCEPTION;
    }
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createDocument(unique_ptr<string> text) {
  return server_->createDocument(std::move(text)).then([this](Try<unique_ptr<string>> result) {
    auto response = std::make_unique<CrudResponse>();
    if (!result.hasException()) {
      response->status = RelevanceStatus::OK;
      response->created = *response.value();
      return std::move(response);
    }
    // can't think of any exceptions we're anticipating here
    response->Status = RelevanceStatus::UNKNOWN_EXCEPTION;
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
    if (result.hasException(DocumentAlreadyExists))
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteDocument(unique_ptr<string> id) {
  return server_->deleteDocument(std::move(id)).then([this](bool deleted) {
    auto res = std::make_unique<CrudResponse>();
    if (deleted) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<unique_ptr<string>> ThriftRelevanceServer::future_getDocument(unique_ptr<string> id) {
  return server_->getDocument(std::move(id));
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createCentroid(unique_ptr<string> centroidId) {
  LOG(INFO) << "createCentroid";
  return server_->createCentroid(std::move(centroidId)).then([](bool created) {
    LOG(INFO) << format("created ? {}", created);
    auto res = std::make_unique<CrudResponse>();
    if (created) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::CLASSIFIER_ALREADY_EXISTS;
    }
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteCentroid(unique_ptr<string> centroidId) {
  return server_->deleteCentroid(std::move(centroidId)).then([](bool deleted) {
    auto res = std::make_unique<CrudResponse>();
    if (deleted) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::CLASSIFIER_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<unique_ptr<ListCentroidDocumentsResponse>> ThriftRelevanceServer::future_listAllDocumentsForCentroid(unique_ptr<string> centroidId) {
  return server_->listAllDocumentsForCentroid(std::move(centroidId)).then([](unique_ptr<vector<string>> documentIds) {
    LOG(INFO) << "response size: " << documentIds->size();
    auto result = std::make_unique<ListCentroidDocumentsResponse>();
    result->status = RelevanceStatus::OK;
    vector<string> docs = *documentIds;
    result->documents = std::move(docs);
    return std::move(result);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_addDocumentToCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->addDocumentToCentroid(std::move(centroidId), std::move(docId)).then([](bool added) {
    auto res = std::make_unique<CrudResponse>();
    if (added) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_removeDocumentFromCentroid(unique_ptr<string> centroidId, unique_ptr<string> docId) {
  return server_->removeDocumentFromCentroid(std::move(centroidId), std::move(docId)).then([](bool removed) {
    auto res = std::make_unique<CrudResponse>();
    if (removed) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<bool> ThriftRelevanceServer::future_recomputeCentroid(unique_ptr<string> centroidId) {
  return server_->recomputeCentroid(std::move(centroidId));
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listAllCentroids() {
  return server_->listAllCentroids();
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listAllDocuments() {
  return server_->listAllDocuments();
}
