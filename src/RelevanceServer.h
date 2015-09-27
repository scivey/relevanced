#pragma once
#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>

#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "RelevanceScoreWorker.h"
#include <folly/Optional.h>
#include "persistence/PersistenceService.h"
#include "DocumentProcessingWorker.h"
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
  virtual folly::Future<double> getRelevanceForDoc(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<double> getRelevanceForText(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<std::unique_ptr<std::string>> createDocument(std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<std::unique_ptr<std::string>> createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) = 0;
  virtual folly::Future<bool> deleteDocument(std::unique_ptr<std::string> id) = 0;
  virtual folly::Future<std::unique_ptr<std::string>> getDocument(std::unique_ptr<std::string> id) = 0;
  virtual folly::Future<bool> createClassifier(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<bool> deleteClassifier(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listAllClassifierDocuments(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<bool> addPositiveDocumentToClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<bool> addNegativeDocumentToClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<bool> removeDocumentFromClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<bool> recompute(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listClassifiers() = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listDocuments() = 0;
  virtual folly::Future<int> getClassifierSize(std::unique_ptr<std::string> collId) = 0;
  virtual ~RelevanceServerIf() = default;
};

class RelevanceServer: public RelevanceServerIf {
  shared_ptr<persistence::PersistenceServiceIf> persistence_;
  shared_ptr<DocumentProcessingWorkerIf> processingWorker_;
  shared_ptr<RelevanceScoreWorkerIf> scoreWorker_;
  folly::Future<std::unique_ptr<std::string>> internalCreateDocumentWithID(std::string id, std::string text);
public:
  RelevanceServer(
    shared_ptr<RelevanceScoreWorkerIf> scoreWorker,
    shared_ptr<DocumentProcessingWorkerIf> docProcessingWorker,
    shared_ptr<persistence::PersistenceServiceIf> persistenceSv
  );
  void ping() override;
  folly::Future<double> getRelevanceForDoc(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<double> getRelevanceForText(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> text) override;
  folly::Future<std::unique_ptr<std::string>> createDocument(std::unique_ptr<std::string> text) override;
  folly::Future<std::unique_ptr<std::string>> createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) override;
  folly::Future<bool> deleteDocument(std::unique_ptr<std::string> id) override;
  folly::Future<std::unique_ptr<std::string>> getDocument(std::unique_ptr<std::string> id) override;
  folly::Future<bool> createClassifier(std::unique_ptr<std::string> collId) override;
  folly::Future<bool> deleteClassifier(std::unique_ptr<std::string> collId) override;
  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listAllClassifierDocuments(std::unique_ptr<std::string> collId) override;
  folly::Future<bool> addPositiveDocumentToClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<bool> addNegativeDocumentToClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<bool> removeDocumentFromClassifier(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<bool> recompute(std::unique_ptr<std::string> collId) override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listClassifiers() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listDocuments() override;
  folly::Future<int> getClassifierSize(std::unique_ptr<std::string> collId) override;
};