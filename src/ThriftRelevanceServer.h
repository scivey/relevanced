#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "RelevanceScoreWorker.h"
#include <folly/Optional.h>
#include "persistence/PersistenceService.h"
#include "DocumentProcessor.h"
#include "Document.h"
#include "util.h"
#include "RelevanceServer.h"

class ThriftRelevanceServer: public services::RelevanceSvIf {
  std::shared_ptr<RelevanceServerIf> server_;
public:
  ThriftRelevanceServer(
    std::shared_ptr<RelevanceServerIf> server
  );
  void ping();
  folly::Future<unique_ptr<services::DocumentRelevanceResponse>>
    future_getRelevanceForDoc(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<services::DocumentRelevanceResponse>>
    future_getRelevanceForText(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_createDocument(std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_deleteDocument(std::unique_ptr<std::string> id) override;

  folly::Future<std::unique_ptr<std::string>>
    future_getDocument(std::unique_ptr<std::string> id) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_createClassifier(std::unique_ptr<std::string> collId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_deleteClassifier(std::unique_ptr<std::string> collId) override;

  folly::Future<unique_ptr<services::ListClassifierDocumentsResponse>>
    future_listAllClassifierDocuments(std::unique_ptr<std::string> collId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_addPositiveDocumentToClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_addNegativeDocumentToClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_removeDocumentFromClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;

  folly::Future<bool> future_recompute(std::unique_ptr<std::string> collId) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>> future_listClassifiers() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> future_listDocuments() override;

  folly::Future<unique_ptr<services::GetClassifierSizeResponse>>
    future_getClassifierSize(std::unique_ptr<std::string> collId) override;
};
