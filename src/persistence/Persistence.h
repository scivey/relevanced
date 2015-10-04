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
#include "models/WordVector.h"

#include "persistence/SyncPersistence.h"

namespace relevanced {
namespace persistence {

class PersistenceIf {
public:
  virtual folly::Future<bool> doesDocumentExist(const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> saveDocument(std::shared_ptr<models::WordVector> doc) = 0;
  virtual folly::Future<folly::Try<bool>> deleteDocument(const std::string &id) = 0;
  virtual folly::Future<std::vector<std::string>> listAllDocuments() = 0;
  virtual folly::Future<folly::Try<std::shared_ptr<models::WordVector>>> loadDocument(const std::string&) = 0;
  virtual folly::Future<folly::Optional<std::shared_ptr<models::WordVector>>> loadDocumentOption(const std::string&) = 0;

  virtual folly::Future<bool> doesCentroidExist(const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> createNewCentroid(const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> deleteCentroid(const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> saveCentroid(const std::string &id, std::shared_ptr<models::WordVector>) = 0;
  virtual folly::Future<folly::Try<std::shared_ptr<models::WordVector>>> loadCentroid(const std::string &id) = 0;
  virtual folly::Future<folly::Optional<std::shared_ptr<models::WordVector>>> loadCentroidOption(const std::string &id) = 0;
  virtual folly::Future<std::vector<std::string>> listAllCentroids() = 0;

  virtual folly::Future<folly::Try<bool>> addDocumentToCentroid(const std::string&, const std::string&) = 0;
  virtual folly::Future<folly::Try<bool>> removeDocumentFromCentroid(const std::string&, const std::string&) = 0;
  virtual folly::Future<folly::Try<bool>> doesCentroidHaveDocument(const std::string&, const std::string&) = 0;
  virtual folly::Future<folly::Try<std::vector<std::string>>>
    listAllDocumentsForCentroid(const std::string&) = 0;
  virtual folly::Future<folly::Optional<std::vector<std::string>>>
    listAllDocumentsForCentroidOption(const std::string&) = 0;

  virtual ~PersistenceIf() = default;
};

class Persistence: public PersistenceIf {
  util::UniquePointer<SyncPersistenceIf> syncHandle_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
public:
  Persistence(
    util::UniquePointer<SyncPersistenceIf> syncHandle,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
  );
  folly::Future<bool> doesDocumentExist(const std::string &id) override;
  folly::Future<folly::Try<bool>> saveDocument(std::shared_ptr<models::WordVector> doc) override;
  folly::Future<folly::Try<bool>> deleteDocument(const std::string &id) override;
  folly::Future<std::vector<std::string>> listAllDocuments() override;
  folly::Future<folly::Try<std::shared_ptr<models::WordVector>>> loadDocument(const std::string&) override;
  folly::Future<folly::Optional<std::shared_ptr<models::WordVector>>> loadDocumentOption(const std::string&) override;

  folly::Future<bool> doesCentroidExist(const std::string &id) override;
  folly::Future<folly::Try<bool>> createNewCentroid(const std::string &id) override;
  folly::Future<folly::Try<bool>> deleteCentroid(const std::string &id) override;
  folly::Future<folly::Try<bool>> saveCentroid(const std::string &id, std::shared_ptr<models::WordVector>) override;
  folly::Future<folly::Try<std::shared_ptr<models::WordVector>>> loadCentroid(const std::string &id) override;
  folly::Future<folly::Optional<std::shared_ptr<models::WordVector>>> loadCentroidOption(const std::string &id) override;
  folly::Future<std::vector<std::string>> listAllCentroids() override;

  folly::Future<folly::Try<bool>> addDocumentToCentroid(const std::string&, const std::string&) override;
  folly::Future<folly::Try<bool>> removeDocumentFromCentroid(const std::string&, const std::string&) override;
  folly::Future<folly::Try<bool>> doesCentroidHaveDocument(const std::string&, const std::string&) override;
  folly::Future<folly::Try<std::vector<std::string>>>
    listAllDocumentsForCentroid(const std::string&) override;
  folly::Future<folly::Optional<std::vector<std::string>>>
    listAllDocumentsForCentroidOption(const std::string&) override;
};


} // persistence
} // relevanced

