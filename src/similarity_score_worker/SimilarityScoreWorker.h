#pragma once

#include <memory>
#include <vector>
#include <cassert>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include <folly/Synchronized.h>

#include "declarations.h"
#include "util/util.h"

namespace relevanced {
namespace similarity_score_worker {

class SimilarityScoreWorkerIf {
 public:
  virtual void initialize() = 0;
  virtual folly::Future<bool> reloadCentroid(std::string id) = 0;
  virtual folly::Future<folly::Try<double>> getDocumentSimilarity(
      std::string centroidId, models::ProcessedDocument *doc) = 0;
  virtual folly::Future<folly::Try<double>> getDocumentSimilarity(
      std::string centroidId,
      std::shared_ptr<models::ProcessedDocument> doc) = 0;
  virtual folly::Future<folly::Try<double>> getCentroidSimilarity(
      std::string centroid1Id, std::string centroid2Id) = 0;
};

/**
 * `SimilarityScoreWorker` is responsible for calculating all
 *  similarity scores.
 * This really involves two responsibilities:
 * - It does the actual similarity computation, i.e. it calls the appropriate
 *   centroid's `score` method from a thread in its dedicated pool.
 * - It keeps all centroids loaded in memory so it can quickly response to
 *   requests, and reloads them whenever instructed to make sure they stay
 *   current.
 *
 * In practice, similarity scores are requested by the parent
 * `RelevanceServer` in response to a corresponding client request.
 * Similarly, requests to reload the centroid are sent by the
 * `RelevanceServer` when it becomes aware of new data.
 *
 */

class SimilarityScoreWorker : public SimilarityScoreWorkerIf {
 protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  std::shared_ptr<persistence::CentroidMetadataDbIf> centroidMetadataDb_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
      threadPool_;
  folly::Synchronized<std::map<std::string, std::shared_ptr<models::Centroid>>>
      centroids_;

 public:
  SimilarityScoreWorker(
      std::shared_ptr<persistence::PersistenceIf> persistence,
      std::shared_ptr<persistence::CentroidMetadataDbIf> metadataDb,
      std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
          threadPool);
  void setLoadedCentroid_(const std::string &id,
                          std::shared_ptr<models::Centroid>);
  folly::Optional<std::shared_ptr<models::Centroid>> getLoadedCentroid_(
      const std::string &id);
  void initialize() override;
  folly::Future<bool> reloadCentroid(std::string id) override;
  folly::Future<folly::Try<double>> getDocumentSimilarity(
      std::string centroidId, models::ProcessedDocument *doc) override;
  folly::Future<folly::Try<double>> getDocumentSimilarity(
      std::string centroidId,
      std::shared_ptr<models::ProcessedDocument> doc) override;
  folly::Future<folly::Try<double>> getCentroidSimilarity(
      std::string centroid1Id, std::string centroid2Id) override;
};

} // similarity_score_worker
} // relevanced
