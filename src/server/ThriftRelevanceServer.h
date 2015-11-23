#pragma once
#include <string>
#include <memory>
#include <map>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include "gen-cpp2/Relevanced.h"
#include "declarations.h"

namespace relevanced {
namespace server {

/**
 * `ThriftRelevanceServer` is not really responsible for relevanced's
 * core functionality.
 *
 * Its job is to proxy requests back to its injected `RelevanceServerIf`
 * instance, and then interpret the responses in a way that makes sense
 * for the defined Thrift protocol.
 *
 */

class ThriftRelevanceServer : public thrift_protocol::RelevancedSvIf {
  std::shared_ptr<RelevanceServerIf> server_;

 public:
  ThriftRelevanceServer(std::shared_ptr<RelevanceServerIf> server);
  void ping() override;

  folly::Future<std::unique_ptr<std::map<std::string, std::string>>>
  future_getServerMetadata() override;

  folly::Future<double> future_getDocumentSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> docId) override;

  folly::Future<std::unique_ptr<thrift_protocol::MultiSimilarityResponse>>
  future_multiGetTextSimilarity(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language) override;

  folly::Future<std::unique_ptr<thrift_protocol::MultiSimilarityResponse>>
  future_multiGetDocumentSimilarity(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      std::unique_ptr<std::string> docId) override;

  folly::Future<double> future_getTextSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language) override;

  folly::Future<double> future_getCentroidSimilarity(
      std::unique_ptr<std::string> centroid1Id,
      std::unique_ptr<std::string> centroid2Id) override;

  folly::Future<std::unique_ptr<thrift_protocol::CreateDocumentResponse>>
  future_createDocument(
    std::unique_ptr<std::string> text,
    thrift_protocol::Language lang
  ) override;

  folly::Future<std::unique_ptr<thrift_protocol::CreateDocumentResponse>>
  future_createDocumentWithID(
    std::unique_ptr<std::string> id,
    std::unique_ptr<std::string> text,
    thrift_protocol::Language lang
  ) override;

  folly::Future<std::unique_ptr<thrift_protocol::DeleteDocumentResponse>>
  future_deleteDocument(std::unique_ptr<std::string> id) override;

  folly::Future<std::unique_ptr<thrift_protocol::CreateCentroidResponse>>
  future_createCentroid(std::unique_ptr<thrift_protocol::CreateCentroidRequest> centroidId) override;

  folly::Future<std::unique_ptr<thrift_protocol::MultiCreateCentroidsResponse>>
  future_multiCreateCentroids(std::unique_ptr<thrift_protocol::MultiCreateCentroidsRequest> request) override;

  folly::Future<std::unique_ptr<thrift_protocol::DeleteCentroidResponse>>
  future_deleteCentroid(std::unique_ptr<thrift_protocol::DeleteCentroidRequest> request) override;

  folly::Future<std::unique_ptr<thrift_protocol::MultiDeleteCentroidsResponse>>
  future_multiDeleteCentroids(std::unique_ptr<thrift_protocol::MultiDeleteCentroidsRequest> request) override;

  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidDocumentsResponse>>
  future_listAllDocumentsForCentroid(
      std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidDocumentsResponse>>
  future_listCentroidDocumentRange(std::unique_ptr<std::string> centroidId, int64_t offset, int64_t count) override;
  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidDocumentsResponse>>
  future_listCentroidDocumentRangeFromID(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId, int64_t count) override;


  folly::Future<std::unique_ptr<thrift_protocol::AddDocumentsToCentroidResponse>>
  future_addDocumentsToCentroid(
    std::unique_ptr<thrift_protocol::AddDocumentsToCentroidRequest> request
  ) override;

  folly::Future<std::unique_ptr<thrift_protocol::RemoveDocumentsFromCentroidResponse>>
  future_removeDocumentsFromCentroid(
    std::unique_ptr<thrift_protocol::RemoveDocumentsFromCentroidRequest> request
  ) override;

  folly::Future<std::unique_ptr<thrift_protocol::JoinCentroidResponse>>
    future_joinCentroid(
        std::unique_ptr<thrift_protocol::JoinCentroidRequest> request
    ) override;

  folly::Future<std::unique_ptr<thrift_protocol::MultiJoinCentroidsResponse>>
    future_multiJoinCentroids(
      std::unique_ptr<thrift_protocol::MultiJoinCentroidsRequest> request
    ) override;

  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidsResponse>>
  future_listAllCentroids() override;
  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidsResponse>>
  future_listCentroidRange(int64_t offset, int64_t count) override;
  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidsResponse>>
  future_listCentroidRangeFromID(std::unique_ptr<std::string> startId, int64_t count) override;


  folly::Future<std::unique_ptr<thrift_protocol::ListDocumentsResponse>>
  future_listAllDocuments() override;
  folly::Future<std::unique_ptr<thrift_protocol::ListDocumentsResponse>>
  future_listUnusedDocuments(int64_t count) override;
  folly::Future<std::unique_ptr<thrift_protocol::ListDocumentsResponse>>
  future_listDocumentRange(int64_t offset, int64_t count) override;
  folly::Future<std::unique_ptr<thrift_protocol::ListDocumentsResponse>>
  future_listDocumentRangeFromID(std::unique_ptr<std::string> startId, int64_t count) override;


  folly::Future<folly::Unit> future_debugEraseAllData() override;

  folly::Future<std::unique_ptr<thrift_protocol::CentroidDTO>>
  future_debugGetFullCentroid(std::unique_ptr<std::string>) override;

  folly::Future<std::unique_ptr<thrift_protocol::ProcessedDocumentDTO>>
  future_debugGetFullProcessedDocument(std::unique_ptr<std::string>) override;

};

} // server
} // relevanced
