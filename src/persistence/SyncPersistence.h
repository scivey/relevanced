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

#include "persistence/exceptions.h"
#include "persistence/RockHandle.h"
#include "models/WordVector.h"
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "util/util.h"

namespace relevanced {
namespace persistence {

class SyncPersistenceIf {
public:
  virtual bool doesDocumentExist(const std::string &id) = 0;
  virtual folly::Try<bool> saveDocument(std::shared_ptr<models::ProcessedDocument> doc) = 0;
  virtual folly::Try<bool> deleteDocument(const std::string &id) = 0;
  virtual std::vector<std::string> listAllDocuments() = 0;
  virtual folly::Try<std::shared_ptr<models::ProcessedDocument>> loadDocument(const std::string&) = 0;
  virtual folly::Optional<std::shared_ptr<models::ProcessedDocument>> loadDocumentOption(const std::string&) = 0;

  virtual bool doesCentroidExist(const std::string &id) = 0;
  virtual folly::Try<bool> createNewCentroid(const std::string &id) = 0;
  virtual folly::Try<bool> deleteCentroid(const std::string &id) = 0;
  virtual folly::Try<bool> saveCentroid(const std::string &id, std::shared_ptr<models::Centroid>) = 0;
  virtual folly::Try<std::shared_ptr<models::Centroid>> loadCentroid(const std::string &id) = 0;
  virtual folly::Optional<std::shared_ptr<models::Centroid>> loadCentroidOption(const std::string &id) = 0;
  virtual std::vector<std::string> listAllCentroids() = 0;

  virtual folly::Try<bool> addDocumentToCentroid(const std::string&, const std::string&) = 0;
  virtual folly::Try<bool> removeDocumentFromCentroid(const std::string&, const std::string&) = 0;
  virtual folly::Try<bool> doesCentroidHaveDocument(const std::string&, const std::string&) = 0;
  virtual folly::Try<std::vector<std::string>> listAllDocumentsForCentroid(const std::string&) = 0;
  virtual folly::Optional<std::vector<std::string>> listAllDocumentsForCentroidOption(const std::string&) = 0;

  virtual ~SyncPersistenceIf() = default;
};

class SyncPersistence: public SyncPersistenceIf {
  util::UniquePointer<RockHandleIf> rockHandle_;
  SyncPersistence(SyncPersistence const&) = delete;
  void operator=(SyncPersistence const&) = delete;
  folly::Optional<models::ProcessedDocument*> loadDocumentRaw(const std::string &id);
  folly::Optional<models::Centroid*> loadCentroidRaw(const std::string &id);
  std::vector<std::string> listAllDocumentsForCentroidRaw(const std::string&);

public:
  SyncPersistence(
    util::UniquePointer<RockHandleIf> rockHandle
  );
  bool doesDocumentExist(const std::string &id) override;
  folly::Try<bool> saveDocument(models::ProcessedDocument *doc);
  folly::Try<bool> saveDocument(std::shared_ptr<models::ProcessedDocument> doc) override;
  folly::Try<bool> deleteDocument(const std::string &id) override;
  std::vector<std::string> listAllDocuments() override;
  folly::Try<std::shared_ptr<models::ProcessedDocument>> loadDocument(const std::string&) override;
  folly::Optional<std::shared_ptr<models::ProcessedDocument>> loadDocumentOption(const std::string&) override;

  bool doesCentroidExist(const std::string &id) override;
  folly::Try<bool> createNewCentroid(const std::string &id) override;
  folly::Try<bool> deleteCentroid(const std::string &id) override;
  folly::Try<bool> saveCentroid(const std::string &id, models::Centroid*);
  folly::Try<bool> saveCentroid(const std::string &id, std::shared_ptr<models::Centroid>) override;
  folly::Try<std::shared_ptr<models::Centroid>> loadCentroid(const std::string &id) override;
  folly::Optional<std::shared_ptr<models::Centroid>> loadCentroidOption(const std::string &id) override;
  std::vector<std::string> listAllCentroids() override;

  folly::Try<bool> addDocumentToCentroid(const std::string&, const std::string&) override;
  folly::Try<bool> removeDocumentFromCentroid(const std::string&, const std::string&) override;
  folly::Try<bool> doesCentroidHaveDocument(const std::string&, const std::string&) override;
  folly::Try<std::vector<std::string>>
    listAllDocumentsForCentroid(const std::string&) override;
  folly::Optional<std::vector<std::string>>
    listAllDocumentsForCentroidOption(const std::string&) override;
};

} // persistence
} // relevanced

