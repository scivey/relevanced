#pragma once

#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/Synchronized.h>
#include <vector>
#include <cassert>
#include "persistence/DocumentDB.h"
#include "persistence/CentroidDB.h"
#include "persistence/CollectionDB.h"
#include "CentroidManager.h"
#include "ProcessedCentroid.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "util.h"

class RelevanceScoreWorkerIf {
public:
  virtual void initialize() = 0;
  virtual folly::Future<bool> reloadCentroid(std::string id) = 0;
  virtual folly::Future<double> getRelevanceForDoc(std::string collectionId, ProcessedDocument *doc) = 0;
  virtual folly::Future<double> getRelevanceForDoc(std::string collectionId, std::shared_ptr<ProcessedDocument> doc) = 0;
  virtual folly::Future<double> getRelevanceForText(std::string collectionId, std::string text) = 0;
  virtual folly::Future<bool> recompute(std::string collectionId) = 0;
  virtual void triggerUpdate(std::string collectionId) = 0;
};

class RelevanceScoreWorker: public RelevanceScoreWorkerIf {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  std::shared_ptr<CentroidManagerIf> centroidManager_;
  std::shared_ptr<DocumentProcessorIf> docProcessor_;
  wangle::FutureExecutor<wangle::CPUThreadPoolExecutor> threadPool_ {4};
  folly::Synchronized<std::map<std::string, std::shared_ptr<ProcessedCentroid>>> centroids_;
  folly::Optional<std::shared_ptr<ProcessedCentroid>> getLoadedCentroid_(const std::string &id);
public:
  RelevanceScoreWorker(
    std::shared_ptr<persistence::PersistenceServiceIf> persistence,
    std::shared_ptr<CentroidManagerIf> centroidManager,
    std::shared_ptr<DocumentProcessorIf> docProcessor
  );
  void initialize() override;
  folly::Future<bool> reloadCentroid(std::string id) override;
  folly::Future<double> getRelevanceForDoc(std::string collectionId, ProcessedDocument *doc) override;
  folly::Future<double> getRelevanceForDoc(std::string collectionId, std::shared_ptr<ProcessedDocument> doc) override;
  folly::Future<double> getRelevanceForText(std::string collectionId, std::string text) override;
  folly::Future<bool> recompute(std::string collectionId) override;
  void triggerUpdate(std::string collectionId) override;
};
