#pragma once
#include <string>
#include <memory>
#include <map>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include "gen-cpp2/Relevanced.h"
#include "server/RelevanceServer.h"

namespace relevanced {
namespace server {

/**
 * `ThriftRelevanceServer` is not really responsible for relevanced's
 * core functionality.
 *
 * Its job is to proxy requests back to its injected `RelevanceServerIf` instance,
 * and then interpret the responses in a way that makes sense for the defined
 * Thrift protocol.
 *
 * A major aspect of this is translating wrapped exceptions like `CentroidDoesNotExist`
 * and `DocumentDoesNotExist` into corresponding status codes for the client, e.g.
 * `RelevanceStatus::CENTROID_DOES_NOT_EXIST`.
 */

class ThriftRelevanceServer: public thrift_protocol::RelevancedSvIf {
  std::shared_ptr<RelevanceServerIf> server_;
public:
  ThriftRelevanceServer(
    std::shared_ptr<RelevanceServerIf> server
  );
  void ping();

  folly::Future<unique_ptr<std::map<std::string, std::string>>>
    future_getServerMetadata() override;

  folly::Future<unique_ptr<thrift_protocol::SimilarityResponse>>
    future_getDocumentSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<thrift_protocol::MultiSimilarityResponse>>
    future_multiGetTextSimilarity(std::unique_ptr<std::vector<std::string>> centroidId, std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<thrift_protocol::SimilarityResponse>>
    future_getTextSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<thrift_protocol::SimilarityResponse>>
    future_getCentroidSimilarity(std::unique_ptr<std::string> centroid1Id, std::unique_ptr<std::string> centroid2Id) override;

  folly::Future<unique_ptr<thrift_protocol::CrudResponse>>
    future_createDocument(std::unique_ptr<std::string> text) override;

  folly::Future<unique_ptr<thrift_protocol::CrudResponse>>
    future_createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) override;

  folly::Future<std::unique_ptr<thrift_protocol::CrudResponse>>
    future_deleteDocument(std::unique_ptr<std::string> id) override;

  folly::Future<std::unique_ptr<thrift_protocol::GetDocumentResponse>>
    future_getDocument(std::unique_ptr<std::string> id) override;

  folly::Future<unique_ptr<thrift_protocol::CrudResponse>>
    future_createCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<unique_ptr<thrift_protocol::CrudResponse>>
    future_deleteCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<unique_ptr<thrift_protocol::ListCentroidDocumentsResponse>>
    future_listAllDocumentsForCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<unique_ptr<thrift_protocol::CrudResponse>>
    future_addDocumentToCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<thrift_protocol::CrudResponse>>
    future_removeDocumentFromCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;

  folly::Future<unique_ptr<thrift_protocol::Status>> future_joinCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>> future_listAllCentroids() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> future_listAllDocuments() override;

};

} // server
} // relevanced
