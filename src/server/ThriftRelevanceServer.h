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
      std::unique_ptr<std::vector<std::string>> centroidId,
      std::unique_ptr<std::string> text) override;

  folly::Future<double> future_getTextSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> text) override;

  folly::Future<double> future_getCentroidSimilarity(
      std::unique_ptr<std::string> centroid1Id,
      std::unique_ptr<std::string> centroid2Id) override;

  folly::Future<std::unique_ptr<thrift_protocol::CreateDocumentResponse>>
  future_createDocument(std::unique_ptr<std::string> text) override;

  folly::Future<std::unique_ptr<thrift_protocol::CreateDocumentResponse>>
  future_createDocumentWithID(std::unique_ptr<std::string> id,
                              std::unique_ptr<std::string> text) override;

  folly::Future<std::unique_ptr<thrift_protocol::DeleteDocumentResponse>>
  future_deleteDocument(std::unique_ptr<std::string> id) override;

  // folly::Future<std::unique_ptr<thrift_protocol::GetDocumentResponse>>
  // future_getDocument(std::unique_ptr<std::string> id) override;

  folly::Future<std::unique_ptr<thrift_protocol::CreateCentroidResponse>>
  future_createCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<thrift_protocol::DeleteCentroidResponse>>
  future_deleteCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidDocumentsResponse>>
  future_listAllDocumentsForCentroid(
      std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<thrift_protocol::AddDocumentToCentroidResponse>>
  future_addDocumentToCentroid(std::unique_ptr<std::string> centroidId,
                               std::unique_ptr<std::string> docId) override;

  folly::Future<
      std::unique_ptr<thrift_protocol::RemoveDocumentFromCentroidResponse>>
  future_removeDocumentFromCentroid(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> docId) override;

  folly::Future<std::unique_ptr<thrift_protocol::JoinCentroidResponse>>
  future_joinCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<thrift_protocol::ListCentroidsResponse>>
  future_listAllCentroids() override;
  folly::Future<std::unique_ptr<thrift_protocol::ListDocumentsResponse>>
  future_listAllDocuments() override;
};

} // server
} // relevanced
