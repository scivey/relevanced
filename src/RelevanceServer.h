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
#include "DocumentProcessor.h"
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
  virtual folly::Future<bool> createCollection(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<bool> deleteCollection(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>>
    listCollectionDocuments(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<bool> addPositiveDocumentToCollection(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<bool> addNegativeDocumentToCollection(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<bool> removeDocumentFromCollection(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) = 0;
  virtual folly::Future<bool> recompute(std::unique_ptr<std::string> collId) = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listCollections() = 0;
  virtual folly::Future<std::unique_ptr<std::vector<std::string>>> listDocuments() = 0;
  virtual folly::Future<int> getCollectionSize(std::unique_ptr<std::string> collId) = 0;
  virtual ~RelevanceServerIf() = default;
};

class RelevanceServer: public RelevanceServerIf {
  shared_ptr<persistence::PersistenceServiceIf> persistence_;
  shared_ptr<DocumentProcessorIf> docProcessor_;
  shared_ptr<RelevanceScoreWorkerIf> scoreWorker_;
  folly::Future<std::unique_ptr<std::string>> internalCreateDocumentWithID(std::string id, std::string text);
public:
  RelevanceServer(
    shared_ptr<RelevanceScoreWorkerIf> scoreWorker,
    shared_ptr<DocumentProcessorIf> docProcessor,
    shared_ptr<persistence::PersistenceServiceIf> persistenceSv
  );
  void ping() override;
  folly::Future<double> getRelevanceForDoc(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<double> getRelevanceForText(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> text) override;
  folly::Future<std::unique_ptr<std::string>> createDocument(std::unique_ptr<std::string> text) override;
  folly::Future<std::unique_ptr<std::string>> createDocumentWithID(std::unique_ptr<std::string> id, std::unique_ptr<std::string> text) override;
  folly::Future<bool> deleteDocument(std::unique_ptr<std::string> id) override;
  folly::Future<std::unique_ptr<std::string>> getDocument(std::unique_ptr<std::string> id) override;
  folly::Future<bool> createCollection(std::unique_ptr<std::string> collId) override;
  folly::Future<bool> deleteCollection(std::unique_ptr<std::string> collId) override;
  folly::Future<std::unique_ptr<std::vector<std::string>>>
    listCollectionDocuments(std::unique_ptr<std::string> collId) override;
  folly::Future<bool> addPositiveDocumentToCollection(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<bool> addNegativeDocumentToCollection(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<bool> removeDocumentFromCollection(std::unique_ptr<std::string> collId, std::unique_ptr<std::string> docId) override;
  folly::Future<bool> recompute(std::unique_ptr<std::string> collId) override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listCollections() override;
  folly::Future<std::unique_ptr<std::vector<std::string>>> listDocuments() override;
  folly::Future<int> getCollectionSize(std::unique_ptr<std::string> collId) override;
};