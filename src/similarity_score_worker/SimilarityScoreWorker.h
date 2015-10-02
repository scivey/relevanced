#pragma once

#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include <folly/Synchronized.h>
#include <vector>
#include <cassert>
#include "persistence/Persistence.h"
#include "models/Centroid.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "models/ProcessedDocument.h"
#include "util/util.h"

namespace relevanced {
namespace similarity_score_worker {

class SimilarityScoreWorkerIf {
public:
  virtual void initialize() = 0;
  virtual folly::Future<bool> reloadCentroid(std::string id) = 0;
  virtual folly::Future<folly::Try<double>> getDocumentSimilarity(std::string centroidId, models::ProcessedDocument *doc) = 0;
  virtual folly::Future<folly::Try<double>> getDocumentSimilarity(std::string centroidId, std::shared_ptr<models::ProcessedDocument> doc) = 0;
};

class SimilarityScoreWorker: public SimilarityScoreWorkerIf {
protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
  folly::Synchronized<std::map<std::string, std::shared_ptr<models::Centroid>>> centroids_;
public:
  SimilarityScoreWorker(
    std::shared_ptr<persistence::PersistenceIf> persistence,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
  );
  void setLoadedCentroid_(const std::string &id, std::shared_ptr<models::Centroid>);
  folly::Optional<std::shared_ptr<models::Centroid>> getLoadedCentroid_(const std::string &id);
  void initialize() override;
  folly::Future<bool> reloadCentroid(std::string id) override;
  folly::Future<folly::Try<double>> getDocumentSimilarity(std::string centroidId, models::ProcessedDocument *doc) override;
  folly::Future<folly::Try<double>> getDocumentSimilarity(std::string centroidId, std::shared_ptr<models::ProcessedDocument> doc) override;
};

} // similarity_score_worker
} // relevanced
