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

#include "declarations.h"
#include "util/util.h"

namespace relevanced {
namespace persistence {

class CentroidMetadataDbIf;

class PersistenceIf {
 public:
  virtual folly::Future<bool>
    doesDocumentExist(std::string id) = 0;

  virtual folly::Future<folly::Try<bool>>
    saveDocument(std::shared_ptr<models::ProcessedDocument> doc) = 0;

  virtual folly::Future<folly::Try<bool>>
    saveNewDocument(std::shared_ptr<models::ProcessedDocument> doc) = 0;

  virtual folly::Future<folly::Try<bool>>
    deleteDocument(std::string id) = 0;

  virtual folly::Future<std::vector<std::string>>
    listAllDocuments() = 0;

  virtual folly::Future<std::vector<std::string>>
    listUnusedDocuments(size_t count) = 0;

  virtual folly::Future<std::vector<std::string>>
    listDocumentRangeFromId(std::string id, size_t count) = 0;

  virtual folly::Future<std::vector<std::string>>
    listDocumentRangeFromOffset(size_t offset, size_t count) = 0;

  virtual folly::Future<folly::Try<std::shared_ptr<models::ProcessedDocument>>>
    loadDocument(std::string ) = 0;

  virtual folly::Future<bool>
    doesCentroidExist(std::string id) = 0;

  virtual folly::Future<folly::Try<bool>>
    createNewCentroid(std::string id) = 0;

  virtual folly::Future<folly::Try<bool>>
    deleteCentroid(std::string id) = 0;

  virtual folly::Future<folly::Try<bool>>
    saveCentroid(std::string id, std::shared_ptr<models::Centroid>) = 0;

  virtual folly::Future<folly::Try<std::shared_ptr<models::Centroid>>>
    loadCentroid(std::string id) = 0;

  virtual folly::Future<folly::Optional<util::UniquePointer<models::Centroid>>>
    loadCentroidUniqueOption(std::string id) = 0;

  virtual folly::Future<std::vector<std::string>>
    listAllCentroids() = 0;

  virtual folly::Future<std::vector<std::string>>
    listCentroidRangeFromOffset(size_t, size_t) = 0;

  virtual folly::Future<std::vector<std::string>>
    listCentroidRangeFromId(std::string, size_t) = 0;

  virtual folly::Future<folly::Try<bool>>
    addDocumentToCentroid(std::string, std::string ) = 0;

  virtual folly::Future<folly::Try<bool>>
    removeDocumentFromCentroid(std::string, std::string ) = 0;

  virtual folly::Future<folly::Try<bool>>
    doesCentroidHaveDocument(std::string, std::string ) = 0;

  virtual folly::Future<folly::Try<std::vector<std::string>>>
    listAllDocumentsForCentroid(std::string ) = 0;

  virtual folly::Future<folly::Optional<std::vector<std::string>>>
    listAllDocumentsForCentroidOption(std::string ) = 0;

  virtual folly::Future<folly::Try<std::vector<std::string>>>
    listCentroidDocumentRangeFromOffset(
      std::string,
      size_t offset,
      size_t limit
    ) = 0;

  virtual folly::Future<folly::Optional<std::vector<std::string>>>
    listCentroidDocumentRangeFromOffsetOption(
      std::string,
      size_t offset,
      size_t limit
    ) = 0;

  virtual folly::Future<folly::Try<std::vector<std::string>>>
    listCentroidDocumentRangeFromDocumentId(
      std::string,
      std::string,
      size_t limit
    ) = 0;

  virtual folly::Future<folly::Optional<std::vector<std::string>>>
    listCentroidDocumentRangeFromDocumentIdOption(
      std::string,
      std::string,
      size_t limit
    ) = 0;

  virtual folly::Future<folly::Optional<std::string>>
    getCentroidMetadata(std::string, std::string ) = 0;

  virtual folly::Future<folly::Try<bool>>
    setCentroidMetadata(
      std::string,
      std::string,
      std::string
    ) = 0;

  virtual folly::Future<folly::Unit>
    debugEraseAllData() = 0;

  virtual ~PersistenceIf() = default;
};

class Persistence : public PersistenceIf {
  util::UniquePointer<SyncPersistenceIf> syncHandle_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
      threadPool_;

 public:
  Persistence(
    util::UniquePointer<SyncPersistenceIf>,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
  );

  folly::Future<bool>
    doesDocumentExist(std::string id) override;

  folly::Future<folly::Try<bool>>
    saveDocument(std::shared_ptr<models::ProcessedDocument> doc) override;

  folly::Future<folly::Try<bool>>
    saveNewDocument(std::shared_ptr<models::ProcessedDocument> doc) override;

  folly::Future<folly::Try<bool>>
    deleteDocument(std::string id) override;

  folly::Future<std::vector<std::string>>
    listAllDocuments() override;

  folly::Future<std::vector<std::string>>
    listUnusedDocuments(size_t count) override;

  folly::Future<std::vector<std::string>>
    listDocumentRangeFromId(std::string id, size_t count) override;

  folly::Future<std::vector<std::string>>
    listDocumentRangeFromOffset(size_t offset, size_t count) override;

  folly::Future<folly::Try<std::shared_ptr<models::ProcessedDocument>>>
    loadDocument(std::string ) override;

  folly::Future<bool>
    doesCentroidExist(std::string id) override;

  folly::Future<folly::Try<bool>>
    createNewCentroid(std::string id) override;

  folly::Future<folly::Try<bool>>
    deleteCentroid(std::string id) override;

  folly::Future<folly::Try<bool>>
    saveCentroid(std::string id, std::shared_ptr<models::Centroid>) override;

  folly::Future<folly::Try<std::shared_ptr<models::Centroid>>>
    loadCentroid(std::string id) override;

  folly::Future<folly::Optional<util::UniquePointer<models::Centroid>>>
    loadCentroidUniqueOption(std::string id) override;

  folly::Future<std::vector<std::string>>
    listAllCentroids() override;

  folly::Future<std::vector<std::string>>
    listCentroidRangeFromOffset(size_t, size_t) override;

  folly::Future<std::vector<std::string>>
    listCentroidRangeFromId(std::string, size_t) override;

  folly::Future<folly::Try<bool>>
    addDocumentToCentroid(std::string, std::string ) override;

  folly::Future<folly::Try<bool>>
    removeDocumentFromCentroid(std::string, std::string ) override;

  folly::Future<folly::Try<bool>>
    doesCentroidHaveDocument(std::string, std::string ) override;

  folly::Future<folly::Try<std::vector<std::string>>>
    listAllDocumentsForCentroid(std::string ) override;

  folly::Future<folly::Optional<std::vector<std::string>>>
    listAllDocumentsForCentroidOption(std::string ) override;

  folly::Future<folly::Try<std::vector<std::string>>>
    listCentroidDocumentRangeFromOffset(
      std::string,
      size_t offset,
      size_t limit
    ) override;

  folly::Future<folly::Optional<std::vector<std::string>>>
    listCentroidDocumentRangeFromOffsetOption(
      std::string,
      size_t offset,
      size_t limit
    ) override;

  folly::Future<folly::Try<std::vector<std::string>>>
    listCentroidDocumentRangeFromDocumentId(
      std::string,
      std::string,
      size_t limit
    ) override;

  folly::Future<folly::Optional<std::vector<std::string>>>
    listCentroidDocumentRangeFromDocumentIdOption(
      std::string,
      std::string,
      size_t limit
    ) override;

  folly::Future<folly::Optional<std::string>>
    getCentroidMetadata(std::string, std::string ) override;

  folly::Future<folly::Try<bool>>
    setCentroidMetadata(
      std::string,
      std::string,
      std::string
    ) override;

  folly::Future<folly::Unit>
    debugEraseAllData() override;

};


} // persistence
} // relevanced
