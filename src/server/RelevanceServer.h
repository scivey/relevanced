#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>

#include <glog/logging.h>
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include <folly/Optional.h>
#include "persistence/Persistence.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "centroid_update_worker/CentroidUpdateWorker.h"

#include "util/util.h"


namespace relevanced {
namespace server {

class RelevanceServerIf {
public:
  virtual void ping() = 0;
  virtual void initialize() = 0;
  virtual folly::Future<std::unique_ptr<std::map<std::string, std::string>>>
    getServerMetadata() = 0;
  virtual folly::Future<folly::Try<double>> getDocumentSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetTextSimilarity(std::unique_ptr<std::vector<std::string>> centroidIds, std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<folly::Try<double>> getTextSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<folly::Try<double>> getCentroidSimilarity(std::unique_ptr<std::string> centroid1Id, std::unique_ptr<std::string> centroid2Id) = 0;
  virtual folly::Future<folly::Try<std::unique_ptr<std::string>>> createDocument(std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<folly::Try<std::unique_ptr<std::string>>> createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<folly::Try<bool>> deleteDocument(std::unique_ptr<std::string> id) = 0;
  virtual folly::Future<folly::Try<std::unique_ptr<std::string>>> getDocument(std::unique_ptr<std::string> id) = 0;
  virtual folly::Future<folly::Try<bool>> createCentroid(std::unique_ptr<std::string> centroidId) = 0;
  virtual folly::Future<folly::Try<bool>> deleteCentroid(std::unique_ptr<std::string> centroidId) = 0;
  virtual folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listAllDocumentsForCentroid(std::unique_ptr<std::string> centroidId) = 0;
  virtual folly::Future<folly::Try<bool>> addDocumentToCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<folly::Try<bool>> removeDocumentFromCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<folly::Try<bool>> joinCentroid(std::unique_ptr<std::string> centroidId) = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listAllCentroids() = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listAllDocuments() = 0;
  virtual ~RelevanceServerIf() = default;
};

/**
 * The central coordinator of the application.  `RelevanceServer`:
 *
 * - Dispatches CRUD requests back to its injected `Persistence` instance.
 * - Routes responses with raw text through its injected `DocumentProcessingWorker`,
     then sends the vectorized documents on to `Persistence` (for operations like
     `createDocument`) or to `SimilarityScoreWorker` (for operations like `getRelevanceForText`).
 * - Keeps track of centroid document membership changes that it has passed on to `Persistence`,
 *   and asks its injected `CentroidUpdateWorker` to recalculate the centroid when appropriate.
 * - Listens for successful centroid updates from its `CentroidUpdateWorker` (via `onUpdate`),
 *   and tells its `SimilarityScoreWorker` instance to reload specific centroids as updated
     models for them become available.
 *
 * This logic is implemented in its own class, rather than in the Thrift
 * server interface implementation, to make it easier to provide alternative
 * ways of interfacing with a single running `RelevanceServer` instance.
 *
 */
class RelevanceServer: public RelevanceServerIf {
  shared_ptr<persistence::PersistenceIf> persistence_;
  shared_ptr<persistence::CentroidMetadataDbIf> centroidMetadataDb_;
  shared_ptr<similarity_score_worker::SimilarityScoreWorkerIf> scoreWorker_;
  shared_ptr<document_processing_worker::DocumentProcessingWorkerIf> processingWorker_;
  shared_ptr<centroid_update_worker::CentroidUpdateWorkerIf> centroidUpdateWorker_;
  folly::Future<folly::Try<std::unique_ptr<std::string>>> internalCreateDocumentWithID(std::string id, std::string text);
public:
  RelevanceServer(
    shared_ptr<persistence::PersistenceIf> persistenceSv,
    shared_ptr<persistence::CentroidMetadataDbIf> metadata,
    shared_ptr<similarity_score_worker::SimilarityScoreWorkerIf> scoreWorker,
    shared_ptr<document_processing_worker::DocumentProcessingWorkerIf> docProcessingWorker,
    shared_ptr<centroid_update_worker::CentroidUpdateWorkerIf> centroidUpdateWorker
  );
  void initialize() override;
  void ping() override;
  folly::Future<std::unique_ptr<std::map<std::string, std::string>>>
    getServerMetadata() override;
  folly::Future<folly::Try<double>> getDocumentSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;
  folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetTextSimilarity(std::unique_ptr<std::vector<std::string>> centroidIds, std::unique_ptr<std::string> text) override;
  folly::Future<folly::Try<double>> getTextSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> text) override;
  folly::Future<folly::Try<double>> getCentroidSimilarity(std::unique_ptr<std::string> centroid1Id, std::unique_ptr<std::string> centroid2Id) override;
  folly::Future<folly::Try<std::unique_ptr<std::string>>> createDocument(std::unique_ptr<std::string> text) override;
  folly::Future<folly::Try<std::unique_ptr<std::string>>> createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) override;
  folly::Future<folly::Try<bool>> deleteDocument(std::unique_ptr<std::string> id) override;
  folly::Future<folly::Try<std::unique_ptr<std::string>>> getDocument(std::unique_ptr<std::string> id) override;
  folly::Future<folly::Try<bool>> createCentroid(std::unique_ptr<std::string> centroidId) override;
  folly::Future<folly::Try<bool>> deleteCentroid(std::unique_ptr<std::string> centroidId) override;
  folly::Future<folly::Try<std::unique_ptr<std::vector<std::string>>>>
    listAllDocumentsForCentroid(std::unique_ptr<std::string> centroidId) override;
  folly::Future<folly::Try<bool>> addDocumentToCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;
  folly::Future<folly::Try<bool>> removeDocumentFromCentroid(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;
  folly::Future<folly::Try<bool>> joinCentroid(std::unique_ptr<std::string> centroidId) override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listAllCentroids() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listAllDocuments() override;
};

} // server
} // relevanced