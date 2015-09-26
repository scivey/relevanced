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

class RelevanceScoreWorker {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  std::shared_ptr<CentroidManager> centroidManager_;
  std::shared_ptr<DocumentProcessor> docProcessor_;
  wangle::FutureExecutor<wangle::CPUThreadPoolExecutor> threadPool_ {4};
  folly::Synchronized<std::map<std::string, ProcessedCentroid*>> centroids_;
public:
  RelevanceScoreWorker(
    std::shared_ptr<persistence::PersistenceServiceIf> persistence,
    std::shared_ptr<CentroidManager> centroidManager,
    std::shared_ptr<DocumentProcessor> docProcessor
  );
  void initialize();
  folly::Future<bool> reloadCentroid(std::string id);
  folly::Future<double> getRelevanceForDoc(std::string collectionId, ProcessedDocument *doc);
  folly::Future<double> getRelevanceForDoc(std::string collectionId, std::shared_ptr<ProcessedDocument> doc);
  folly::Future<double> getRelevanceForText(std::string collectionId, std::string text);
  folly::Future<bool> recompute(std::string collectionId);
};
