#include <string>
#include <memory>
#include <vector>
#include <folly/futures/Future.h>
#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "ThriftRelevanceServer.h"
#include "RelevanceServer.h"

using namespace services;
using namespace std;
using namespace folly;

ThriftRelevanceServer::ThriftRelevanceServer(shared_ptr<RelevanceServerIf> server)
  :server_(server) {}

void ThriftRelevanceServer::ping(){
  server_->ping();
}

Future<unique_ptr<DocumentRelevanceResponse>> ThriftRelevanceServer::future_getRelevanceForDoc(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->getRelevanceForDoc(std::move(collId), std::move(docId)).then([this](double relevance) {
    auto res = std::make_unique<DocumentRelevanceResponse>();
    res->status = RelevanceStatus::OK;
    res->relevance = relevance;
    return std::move(res);
  });
}

Future<unique_ptr<DocumentRelevanceResponse>> ThriftRelevanceServer::future_getRelevanceForText(unique_ptr<string> collId, unique_ptr<string> text) {
  return server_->getRelevanceForText(std::move(collId), std::move(text)).then([this](double relevance) {
    auto res = std::make_unique<DocumentRelevanceResponse>();
    res->status = RelevanceStatus::OK;
    res->relevance = relevance;
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createDocument(unique_ptr<string> text) {
  return server_->createDocument(std::move(text)).then([this](unique_ptr<string> id) {
    auto res = std::make_unique<CrudResponse>();
    res->status = RelevanceStatus::OK;
    res->created = *id;
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) {
  return server_->createDocumentWithID(std::move(id), std::move(text)).then([this](unique_ptr<string> id) {
    auto res = std::make_unique<CrudResponse>();
    res->status = RelevanceStatus::OK;
    res->created = *id;
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

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_createClassifier(unique_ptr<string> collId) {
  return server_->createClassifier(std::move(collId)).then([](bool created) {
    auto res = std::make_unique<CrudResponse>();
    if (created) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::CLASSIFIER_ALREADY_EXISTS;
    }
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_deleteClassifier(unique_ptr<string> collId) {
  return server_->deleteClassifier(std::move(collId)).then([](bool deleted) {
    auto res = std::make_unique<CrudResponse>();
    if (deleted) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::CLASSIFIER_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<unique_ptr<ListClassifierDocumentsResponse>> ThriftRelevanceServer::future_listAllClassifierDocuments(unique_ptr<string> collId) {
  return server_->listAllClassifierDocuments(std::move(collId)).then([](unique_ptr<vector<string>> documentIds) {
    LOG(INFO) << "response size: " << documentIds->size();
    auto result = std::make_unique<ListClassifierDocumentsResponse>();
    result->status = RelevanceStatus::OK;
    vector<string> docs = *documentIds;
    result->documents = std::move(docs);
    return std::move(result);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_addPositiveDocumentToClassifier(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->addPositiveDocumentToClassifier(std::move(collId), std::move(docId)).then([](bool added) {
    auto res = std::make_unique<CrudResponse>();
    if (added) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_addNegativeDocumentToClassifier(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->addNegativeDocumentToClassifier(std::move(collId), std::move(docId)).then([](bool added) {
    auto res = std::make_unique<CrudResponse>();
    if (added) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<unique_ptr<CrudResponse>> ThriftRelevanceServer::future_removeDocumentFromClassifier(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->removeDocumentFromClassifier(std::move(collId), std::move(docId)).then([](bool removed) {
    auto res = std::make_unique<CrudResponse>();
    if (removed) {
      res->status = RelevanceStatus::OK;
    } else {
      res->status = RelevanceStatus::DOCUMENT_DOES_NOT_EXIST;
    }
    return std::move(res);
  });
}

Future<bool> ThriftRelevanceServer::future_recompute(unique_ptr<string> collId) {
  return server_->recompute(std::move(collId));
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listClassifiers() {
  return server_->listClassifiers();
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listDocuments() {
  return server_->listDocuments();
}

Future<unique_ptr<GetClassifierSizeResponse>> ThriftRelevanceServer::future_getClassifierSize(unique_ptr<string> collId) {
  return server_->getClassifierSize(std::move(collId)).then([](int size) {
    auto res = std::make_unique<GetClassifierSizeResponse>();
    res->status = RelevanceStatus::OK;
    res->size = size;
    return std::move(res);
  });
}
