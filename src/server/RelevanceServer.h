#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>
#include <glog/logging.h>
#include <folly/Optional.h>
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "util/util.h"
#include "declarations.h"
namespace relevanced {

namespace server {

class RelevanceServerIf {
 public:
  virtual void ping() = 0;
  virtual void initialize() = 0;

  virtual folly::Future<std::unique_ptr<std::map<std::string, std::string>>>
    getServerMetadata() = 0;

  virtual folly::Future<folly::Try<double>>
    getDocumentSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> docId
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetTextSimilarity(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetDocumentSimilarity(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      std::unique_ptr<std::string> docId
    ) = 0;

  virtual folly::Future<folly::Try<double>>
    getTextSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language
    ) = 0;

  virtual folly::Future<folly::Try<double>>
    getCentroidSimilarity(
      std::unique_ptr<std::string> centroid1Id,
      std::unique_ptr<std::string> centroid2Id
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::string>>>
    createDocument(
      std::unique_ptr<std::string> text,
      thrift_protocol::Language
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::string>>>
    createDocumentWithID(
      std::unique_ptr<std::string> id,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language
    ) = 0;

  virtual folly::Future<folly::Try<bool>>
    deleteDocument(std::unique_ptr<std::string> id, bool ignoreMissing) = 0;

  virtual folly::Future<std::vector<folly::Try<bool>>>
    multiDeleteDocuments(std::unique_ptr<std::vector<std::string>> ids, bool ignoreMissing) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::string>>>
    getDocument(std::unique_ptr<std::string> id) = 0;

  virtual folly::Future<folly::Try<bool>>
    createCentroid(std::unique_ptr<std::string> centroidId, bool ignoreExisting) = 0;

  virtual folly::Future<std::vector<folly::Try<bool>>>
    multiCreateCentroids(std::unique_ptr<std::vector<std::string>> ids, bool ignoreExisting) = 0;

  virtual folly::Future<folly::Try<bool>>
    deleteCentroid(std::unique_ptr<std::string> centroidId, bool ignoreMissing) = 0;

  virtual folly::Future<std::vector<folly::Try<bool>>>
    multiDeleteCentroids(std::unique_ptr<std::vector<std::string>> ids, bool ignoreMissing) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listAllDocumentsForCentroid(std::unique_ptr<std::string> centroidId) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<thrift_protocol::AddDocumentsToCentroidResponse>>>
    addDocumentsToCentroid(
      std::unique_ptr<thrift_protocol::AddDocumentsToCentroidRequest> request
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<thrift_protocol::RemoveDocumentsFromCentroidResponse>>>
    removeDocumentsFromCentroid(
      std::unique_ptr<thrift_protocol::RemoveDocumentsFromCentroidRequest> request
    ) = 0;

  virtual folly::Future<folly::Try<bool>>
    joinCentroid(
      std::unique_ptr<std::string> centroidId,
      bool ignoreMissing
    ) = 0;

  virtual folly::Future<std::unique_ptr<std::vector<folly::Try<bool>>>>
    multiJoinCentroids(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      bool ignoreMissing
    ) = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listAllCentroids() = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listAllDocuments() = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listUnusedDocuments(size_t count) = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listCentroidRange(size_t offset, size_t count) = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listCentroidRangeFromID(
      std::unique_ptr<std::string> centroidId,
      size_t count
    ) = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listDocumentRange(size_t offset, size_t count) = 0;

  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listDocumentRangeFromID(
      std::unique_ptr<std::string> docId,
      size_t count
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listCentroidDocumentRange(
      std::unique_ptr<std::string> centroidId,
      size_t offset,
      size_t count
    ) = 0;

  virtual folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listCentroidDocumentRangeFromID(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> docId,
      size_t count
    ) = 0;

  virtual folly::Future<folly::Unit>
    debugEraseAllData() = 0;

  virtual folly::Future<folly::Try<std::shared_ptr<models::Centroid>>>
    debugGetFullCentroid(std::unique_ptr<std::string> centroidId) = 0;

  virtual folly::Future<folly::Try<std::shared_ptr<models::ProcessedDocument>>>
    debugGetFullProcessedDocument(
      std::unique_ptr<std::string> documentId
    ) = 0;

  virtual ~RelevanceServerIf() = default;

};

/**
 * The central coordinator of the application.  `RelevanceServer`:
 *
 * - Dispatches CRUD requests back to its injected `Persistence` instance.
 * - Routes responses with raw text through its injected
 *   `DocumentProcessingWorker`, then sends the vectorized documents on to
 *   `Persistence` (for operations like `createDocument`) or to
 *   `SimilarityScoreWorker` (for operations like `getRelevanceForText`).
 * - Keeps track of centroid document membership changes that it has passed
 *   on to `Persistence`, and asks its injected `CentroidUpdateWorker` to
 *   recalculate the centroid when appropriate.
 * - Listens for successful centroid updates from its `CentroidUpdateWorker`
 *   (via `onUpdate`), and tells its `SimilarityScoreWorker` instance to
 *   reload specific centroids as updated models for them become available.
 *
 * This logic is implemented in its own class, rather than in the Thrift
 * server interface implementation, to make it easier to provide alternative
 * ways of interfacing with a single running `RelevanceServer` instance.
 *
 */

class RelevanceServer : public RelevanceServerIf {
 protected:
  std::shared_ptr<persistence::PersistenceIf>
    persistence_;

  std::shared_ptr<persistence::CentroidMetadataDbIf>
    centroidMetadataDb_;

  std::shared_ptr<util::ClockIf>
    clock_;

  std::shared_ptr<similarity_score_worker::SimilarityScoreWorkerIf>
    scoreWorker_;

  std::shared_ptr<document_processing_worker::DocumentProcessingWorkerIf>
    processingWorker_;

  std::shared_ptr<centroid_update_worker::CentroidUpdateWorkerIf>
    centroidUpdateWorker_;

  folly::Future<folly::Try<std::unique_ptr<std::string>>>
    internalCreateDocumentWithID(
      std::string id,
      std::string text,
      thrift_protocol::Language
    );

  folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    internalMultiGetDocumentSimilarity(
      std::shared_ptr<std::vector<std::string>> centroidIds,
      std::shared_ptr<models::ProcessedDocument>
    );

 public:
  RelevanceServer(
    std::shared_ptr<persistence::PersistenceIf>,
    std::shared_ptr<persistence::CentroidMetadataDbIf>,
    std::shared_ptr<util::ClockIf>,
    std::shared_ptr<similarity_score_worker::SimilarityScoreWorkerIf>,
    std::shared_ptr<document_processing_worker::DocumentProcessingWorkerIf>,
    std::shared_ptr<centroid_update_worker::CentroidUpdateWorkerIf>
  );

  void initialize() override;

  void ping() override;

  folly::Future<std::unique_ptr<std::map<std::string, std::string>>>
    getServerMetadata() override;

  folly::Future<folly::Try<double>>
    getDocumentSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> docId
    ) override;

  folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetTextSimilarity(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language lang
    ) override;

  folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetDocumentSimilarity(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      std::unique_ptr<std::string> docId
    ) override;

  folly::Future<folly::Try<double>>
    getTextSimilarity(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language lang
    ) override;

  folly::Future<folly::Try<double>>
    getCentroidSimilarity(
      std::unique_ptr<std::string> centroid1Id,
      std::unique_ptr<std::string> centroid2Id
    ) override;

  folly::Future<folly::Try<std::unique_ptr<std::string>>>
    createDocument(
      std::unique_ptr<std::string> text,
      thrift_protocol::Language
    ) override;

  folly::Future<folly::Try<std::unique_ptr<std::string>>>
    createDocumentWithID(
      std::unique_ptr<std::string> id,
      std::unique_ptr<std::string> text,
      thrift_protocol::Language
    ) override;

  folly::Future<folly::Try<bool>>
    deleteDocument(std::unique_ptr<std::string> id, bool ignoreMissing) override;

  folly::Future<std::vector<folly::Try<bool>>>
    multiDeleteDocuments(std::unique_ptr<std::vector<std::string>> ids, bool ignoreMissing) override;

  folly::Future<folly::Try<std::unique_ptr<std::string>>>
    getDocument(std::unique_ptr<std::string> id) override;

  folly::Future<folly::Try<bool>>
    createCentroid(std::unique_ptr<std::string> centroidId, bool ignoreExisting) override;

  folly::Future<std::vector<folly::Try<bool>>>
    multiCreateCentroids(std::unique_ptr<std::vector<std::string>> ids, bool ignoreExisting) override;

  folly::Future<folly::Try<bool>>
    deleteCentroid(
      std::unique_ptr<std::string> centroidId,
      bool ignoreMissing
    ) override;

  folly::Future<std::vector<folly::Try<bool>>>
    multiDeleteCentroids(std::unique_ptr<std::vector<std::string>> ids, bool ignoreMissing) override;

  folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listAllDocumentsForCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<folly::Try<bool>>
    addOneDocumentToCentroid(
      std::string centroidId, std::string documentId, bool ignoreMissingDoc, bool ignoreAlreadyInCentroid
    );

  folly::Future<folly::Try<bool>>
    removeOneDocumentFromCentroid(
      std::string centroidId, std::string documentId, bool ignoreMissingDoc, bool ignoreNotInCentroid
    );

  folly::Future<folly::Try<std::unique_ptr<thrift_protocol::AddDocumentsToCentroidResponse>>>
    addDocumentsToCentroid(
      std::unique_ptr<thrift_protocol::AddDocumentsToCentroidRequest> request
    ) override;

  folly::Future<folly::Try<std::unique_ptr<thrift_protocol::RemoveDocumentsFromCentroidResponse>>>
    removeDocumentsFromCentroid(
      std::unique_ptr<thrift_protocol::RemoveDocumentsFromCentroidRequest> request
    ) override;

  folly::Future<folly::Try<bool>>
    joinCentroid(
      std::string centroidId,
      bool ignoreMissing
    );

  folly::Future<folly::Try<bool>>
    joinCentroid(
      std::unique_ptr<std::string> centroidId,
      bool ignoreMissing
    ) override;

  folly::Future<std::unique_ptr<std::vector<folly::Try<bool>>>>
    multiJoinCentroids(
      std::unique_ptr<std::vector<std::string>> centroidIds,
      bool ignoreMissing
    ) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listAllCentroids() override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listAllDocuments() override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listUnusedDocuments(size_t count) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listCentroidRange(size_t offset, size_t count) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listCentroidRangeFromID(
      std::unique_ptr<std::string> centroidId,
      size_t count
    ) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listDocumentRange(size_t offset, size_t count) override;

  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listDocumentRangeFromID(
      std::unique_ptr<std::string> docId,
      size_t count
    ) override;

  folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listCentroidDocumentRange(
      std::unique_ptr<std::string> centroidId,
      size_t offset,
      size_t count
    ) override;

  folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listCentroidDocumentRangeFromID(
      std::unique_ptr<std::string> centroidId,
      std::unique_ptr<std::string> docId,
      size_t count
    ) override;

  folly::Future<folly::Unit>
    debugEraseAllData() override;

  folly::Future<folly::Try<std::shared_ptr<models::Centroid>>>
    debugGetFullCentroid(std::unique_ptr<std::string> centroidId) override;

  folly::Future<folly::Try<std::shared_ptr<models::ProcessedDocument>>>
    debugGetFullProcessedDocument(
      std::unique_ptr<std::string> documentId
    ) override;

};

} // server
} // relevanced