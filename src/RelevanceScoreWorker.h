#pragma once

#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/Synchronized.h>
#include <vector>
#include <cassert>
#include "persistence/Persistence.h"
#include "CentroidManager.h"
#include "Centroid.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "util.h"

class RelevanceScoreWorkerIf {
public:
  virtual void initialize() = 0;
  virtual folly::Future<bool> reloadCentroid(std::string id) = 0;
  virtual folly::Future<double> getDocumentSimilarity(std::string centroidId, ProcessedDocument *doc) = 0;
  virtual folly::Future<double> getDocumentSimilarity(std::string centroidId, std::shared_ptr<ProcessedDocument> doc) = 0;
  virtual folly::Future<bool> recomputeCentroid(std::string centroidId) = 0;
  virtual void triggerUpdate(std::string centroidId) = 0;
};

class RelevanceScoreWorker: public RelevanceScoreWorkerIf {
protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  std::shared_ptr<CentroidManagerIf> centroidManager_;
  wangle::FutureExecutor<wangle::CPUThreadPoolExecutor> threadPool_ {4};
  folly::Synchronized<std::map<std::string, std::shared_ptr<Centroid>>> centroids_;
  folly::Optional<std::shared_ptr<Centroid>> getLoadedCentroid_(const std::string &id);
public:
  RelevanceScoreWorker(
    std::shared_ptr<persistence::PersistenceIf> persistence,
    std::shared_ptr<CentroidManagerIf> centroidManager
  );
  void initialize() override;
  folly::Future<bool> reloadCentroid(std::string id) override;
  folly::Future<double> getDocumentSimilarity(std::string centroidId, ProcessedDocument *doc) override;
  folly::Future<double> getDocumentSimilarity(std::string centroidId, std::shared_ptr<ProcessedDocument> doc) override;
  folly::Future<bool> recomputeCentroid(std::string centroidId) override;
  void triggerUpdate(std::string centroidId) override;
};
