#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include "gen-cpp2/Relevance.h"
#include "RelevanceServer.h"

class ThriftRelevanceServer: public services::RelevanceSvIf {
  std::shared_ptr<RelevanceServerIf> server_;
public:
  ThriftRelevanceServer(
    std::shared_ptr<RelevanceServerIf> server
  );
  void ping();
  folly::Future<unique_ptr<services::DocumentRelevanceResponse>>
    future_getDocumentSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<services::DocumentMultiRelevanceResponse>>
    future_multiGetTextSimilarity(std::unique_ptr<std::vector<std::string>> centroidId, std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<services::DocumentRelevanceResponse>>
    future_getTextSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_createDocument(std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) override;

  folly::Future<std::unique_ptr<services::CrudResponse>>
    future_deleteDocument(std::unique_ptr<std::string> id) override;

  folly::Future<std::unique_ptr<services::GetDocumentResponse>>
    future_getDocument(std::unique_ptr<std::string> id) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_createCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_deleteCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<unique_ptr<services::ListCentroidDocumentsResponse>>
    future_listAllDocumentsForCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_addDocumentToCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<services::CrudResponse>>
    future_removeDocumentFromCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;

  folly::Future<bool> future_recomputeCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>> future_listAllCentroids() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> future_listAllDocuments() override;

};
