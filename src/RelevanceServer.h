#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "SimilarityScoreWorker.h"
#include <folly/Optional.h>
#include "persistence/Persistence.h"
#include "DocumentProcessingWorker.h"
#include "CentroidUpdateWorker.h"

#include "Document.h"
#include "util.h"

namespace {
  using namespace std;
  using namespace folly;
  using util::UniquePointer;
}

class RelevanceServerIf {
public:
  virtual void ping() = 0;
  virtual void initialize() = 0;
  virtual folly::Future<folly::Try<double>> getDocumentSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetTextSimilarity(std::unique_ptr<std::vector<std::string>> centroidIds, std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<folly::Try<double>> getTextSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> text) = 0;
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
  virtual folly::Future<folly::Try<bool>> recomputeCentroid(std::unique_ptr<std::string> centroidId) = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listAllCentroids() = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listAllDocuments() = 0;
  virtual ~RelevanceServerIf() = default;
};

class RelevanceServer: public RelevanceServerIf {
  shared_ptr<persistence::PersistenceIf> persistence_;
  shared_ptr<SimilarityScoreWorkerIf> scoreWorker_;
  shared_ptr<DocumentProcessingWorkerIf> processingWorker_;
  shared_ptr<CentroidUpdateWorkerIf> centroidUpdateWorker_;
  folly::Future<folly::Try<std::unique_ptr<std::string>>> internalCreateDocumentWithID(std::string id, std::string text);
public:
  RelevanceServer(
    shared_ptr<persistence::PersistenceIf> persistenceSv,
    shared_ptr<SimilarityScoreWorkerIf> scoreWorker,
    shared_ptr<DocumentProcessingWorkerIf> docProcessingWorker,
    shared_ptr<CentroidUpdateWorkerIf> centroidUpdateWorker
  );
  void initialize() override;
  void ping() override;
  folly::Future<folly::Try<double>> getDocumentSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> docId) override;
  folly::Future<folly::Try<std::unique_ptr<std::map<std::string, double>>>>
    multiGetTextSimilarity(std::unique_ptr<std::vector<std::string>> centroidIds, std::unique_ptr<std::string> text) override;
  folly::Future<folly::Try<double>> getTextSimilarity(std::unique_ptr<std::string> centroidId, std::unique_ptr<std::string> text) override;
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
  folly::Future<folly::Try<bool>> recomputeCentroid(std::unique_ptr<std::string> centroidId) override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listAllCentroids() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listAllDocuments() override;
};
