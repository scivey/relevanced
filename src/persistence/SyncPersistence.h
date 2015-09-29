#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glog/logging.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>

#include "exceptions.h"
#include "RockHandle.h"
#include "Centroid.h"
#include "ProcessedDocument.h"
#include "util.h"

namespace persistence {

class SyncPersistenceIf {
public:
  virtual void initialize() = 0;
  virtual bool doesDocumentExist(const std::string &id) = 0;
  virtual folly::Try<bool> saveDocument(std::shared_ptr<ProcessedDocument> doc) = 0;
  virtual folly::Try<bool> deleteDocument(const std::string &id) = 0;
  virtual std::vector<std::string> listAllDocuments() = 0;
  virtual folly::Try<std::shared_ptr<ProcessedDocument>> loadDocument(const std::string&) = 0;
  virtual folly::Optional<std::shared_ptr<ProcessedDocument>> loadDocumentOption(const std::string&) = 0;

  virtual bool doesCentroidExist(const std::string &id) = 0;
  virtual folly::Try<bool> createNewCentroid(const std::string &id) = 0;
  virtual folly::Try<bool> deleteCentroid(const std::string &id) = 0;
  virtual folly::Try<bool> saveCentroid(const std::string &id, std::shared_ptr<Centroid>) = 0;
  virtual folly::Try<std::shared_ptr<Centroid>> loadCentroid(const std::string &id) = 0;
  virtual folly::Optional<std::shared_ptr<Centroid>> loadCentroidOption(const std::string &id) = 0;
  virtual std::vector<std::string> listAllCentroids() = 0;

  virtual folly::Try<bool> addDocumentToCentroid(const std::string&, const std::string&) = 0;
  virtual folly::Try<bool> removeDocumentFromCentroid(const std::string&, const std::string&) = 0;
  virtual folly::Try<bool> doesCentroidHaveDocument(const std::string&, const std::string&) = 0;
  virtual folly::Try<std::vector<std::string>> listAllDocumentsForCentroid(const std::string&) = 0;
  virtual ~SyncPersistenceIf() = default;
};

class SyncPersistence: public SyncPersistenceIf {
  util::UniquePointer<RockHandleIf> rockHandle_;
  SyncPersistence(SyncPersistence const&) = delete;
  void operator=(SyncPersistence const&) = delete;
  folly::Optional<ProcessedDocument*> loadDocumentRaw(const std::string &id);
  folly::Optional<Centroid*> loadCentroidRaw(const std::string &id);

public:
  SyncPersistence(
    util::UniquePointer<RockHandleIf> rockHandle
  );
  void initialize() override;
  bool doesDocumentExist(const std::string &id) override;
  folly::Try<bool> saveDocument(ProcessedDocument *doc);
  folly::Try<bool> saveDocument(std::shared_ptr<ProcessedDocument> doc) override;
  folly::Try<bool> deleteDocument(const std::string &id) override;
  std::vector<std::string> listAllDocuments() override;
  folly::Try<std::shared_ptr<ProcessedDocument>> loadDocument(const std::string&) override;
  folly::Optional<std::shared_ptr<ProcessedDocument>> loadDocumentOption(const std::string&) override;

  bool doesCentroidExist(const std::string &id) override;
  folly::Try<bool> createNewCentroid(const std::string &id) override;
  folly::Try<bool> deleteCentroid(const std::string &id) override;
  folly::Try<bool> saveCentroid(const std::string &id, Centroid*);
  folly::Try<bool> saveCentroid(const std::string &id, std::shared_ptr<Centroid>) override;
  folly::Try<std::shared_ptr<Centroid>> loadCentroid(const std::string &id) override;
  folly::Optional<std::shared_ptr<Centroid>> loadCentroidOption(const std::string &id) override;
  std::vector<std::string> listAllCentroids() override;

  folly::Try<bool> addDocumentToCentroid(const std::string&, const std::string&) override;
  folly::Try<bool> removeDocumentFromCentroid(const std::string&, const std::string&) override;
  folly::Try<bool> doesCentroidHaveDocument(const std::string&, const std::string&) override;
  folly::Try<std::vector<std::string>> listAllDocumentsForCentroid(const std::string&) override;
};

} // persistence
