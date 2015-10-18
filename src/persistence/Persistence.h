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
  virtual folly::Future<bool> doesDocumentExist(const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> saveDocument(
      std::shared_ptr<models::ProcessedDocument> doc) = 0;
  virtual folly::Future<folly::Try<bool>> deleteDocument(
      const std::string &id) = 0;
  virtual folly::Future<std::vector<std::string>> listAllDocuments() = 0;
  virtual folly::Future<std::vector<std::string>> listDocumentRangeFromId(
      const std::string &id, size_t count) = 0;
  virtual folly::Future<std::vector<std::string>> listDocumentRangeFromOffset(
      size_t offset, size_t count) = 0;
  virtual folly::Future<folly::Try<std::shared_ptr<models::ProcessedDocument>>>
  loadDocument(const std::string &) = 0;
  virtual folly::Future<
      folly::Optional<std::shared_ptr<models::ProcessedDocument>>>
  loadDocumentOption(const std::string &) = 0;

  virtual folly::Future<bool> doesCentroidExist(const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> createNewCentroid(
      const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> deleteCentroid(
      const std::string &id) = 0;
  virtual folly::Future<folly::Try<bool>> saveCentroid(
      const std::string &id, std::shared_ptr<models::Centroid>) = 0;
  virtual folly::Future<folly::Try<std::shared_ptr<models::Centroid>>>
  loadCentroid(const std::string &id) = 0;
  virtual folly::Future<folly::Optional<std::shared_ptr<models::Centroid>>>
  loadCentroidOption(const std::string &id) = 0;
  virtual folly::Future<folly::Optional<std::unique_ptr<models::Centroid>>>
  loadCentroidUniqueOption(const std::string &id) = 0;
  virtual folly::Future<std::vector<std::string>> listAllCentroids() = 0;
  virtual folly::Future<std::vector<std::string>> listCentroidRangeFromOffset(
      size_t, size_t) = 0;
  virtual folly::Future<std::vector<std::string>> listCentroidRangeFromId(
      const std::string &, size_t) = 0;

  virtual folly::Future<folly::Try<bool>> addDocumentToCentroid(
      const std::string &, const std::string &) = 0;
  virtual folly::Future<folly::Try<bool>> removeDocumentFromCentroid(
      const std::string &, const std::string &) = 0;
  virtual folly::Future<folly::Try<bool>> doesCentroidHaveDocument(
      const std::string &, const std::string &) = 0;

  virtual folly::Future<folly::Try<std::vector<std::string>>>
  listAllDocumentsForCentroid(const std::string &) = 0;
  virtual folly::Future<folly::Optional<std::vector<std::string>>>
  listAllDocumentsForCentroidOption(const std::string &) = 0;

  virtual folly::Future<folly::Try<std::vector<std::string>>>
  listCentroidDocumentRangeFromOffset(const std::string &,
                                      size_t offset,
                                      size_t limit) = 0;
  virtual folly::Future<folly::Optional<std::vector<std::string>>>
  listCentroidDocumentRangeFromOffsetOption(const std::string &,
                                            size_t offset,
                                            size_t limit) = 0;

  virtual folly::Future<folly::Try<std::vector<std::string>>>
  listCentroidDocumentRangeFromDocumentId(const std::string &,
                                          const std::string &,
                                          size_t limit) = 0;
  virtual folly::Future<folly::Optional<std::vector<std::string>>>
  listCentroidDocumentRangeFromDocumentIdOption(const std::string &,
                                                const std::string &,
                                                size_t limit) = 0;

  virtual folly::Future<folly::Optional<std::string>> getCentroidMetadata(
      const std::string &, const std::string &) = 0;
  virtual folly::Future<folly::Try<bool>> setCentroidMetadata(
      const std::string &, const std::string &, std::string) = 0;

  virtual folly::Future<folly::Unit> debugEraseAllData() = 0;

  virtual ~PersistenceIf() = default;
};

class Persistence : public PersistenceIf {
  util::UniquePointer<SyncPersistenceIf> syncHandle_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
      threadPool_;

 public:
  Persistence(
      util::UniquePointer<SyncPersistenceIf> syncHandle,
      std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
          threadPool);

  folly::Future<bool> doesDocumentExist(const std::string &id) override;
  folly::Future<folly::Try<bool>> saveDocument(
      std::shared_ptr<models::ProcessedDocument> doc) override;
  folly::Future<folly::Try<bool>> deleteDocument(
      const std::string &id) override;
  folly::Future<std::vector<std::string>> listAllDocuments() override;
  folly::Future<std::vector<std::string>> listDocumentRangeFromId(
      const std::string &id, size_t count) override;
  folly::Future<std::vector<std::string>> listDocumentRangeFromOffset(
      size_t offset, size_t count) override;
  folly::Future<folly::Try<std::shared_ptr<models::ProcessedDocument>>>
  loadDocument(const std::string &) override;
  folly::Future<folly::Optional<std::shared_ptr<models::ProcessedDocument>>>
  loadDocumentOption(const std::string &) override;

  folly::Future<bool> doesCentroidExist(const std::string &id) override;
  folly::Future<folly::Try<bool>> createNewCentroid(
      const std::string &id) override;
  folly::Future<folly::Try<bool>> deleteCentroid(
      const std::string &id) override;
  folly::Future<folly::Try<bool>> saveCentroid(
      const std::string &id, std::shared_ptr<models::Centroid>) override;
  folly::Future<folly::Try<std::shared_ptr<models::Centroid>>> loadCentroid(
      const std::string &id) override;
  folly::Future<folly::Optional<std::shared_ptr<models::Centroid>>>
  loadCentroidOption(const std::string &id) override;

  folly::Future<folly::Optional<std::unique_ptr<models::Centroid>>>
  loadCentroidUniqueOption(const std::string &id) override;
  folly::Future<std::vector<std::string>> listAllCentroids() override;
  folly::Future<std::vector<std::string>> listCentroidRangeFromOffset(
      size_t, size_t) override;
  folly::Future<std::vector<std::string>> listCentroidRangeFromId(
      const std::string &, size_t) override;

  folly::Future<folly::Try<bool>> addDocumentToCentroid(
      const std::string &, const std::string &) override;
  folly::Future<folly::Try<bool>> removeDocumentFromCentroid(
      const std::string &, const std::string &) override;
  folly::Future<folly::Try<bool>> doesCentroidHaveDocument(
      const std::string &, const std::string &) override;
  folly::Future<folly::Try<std::vector<std::string>>>
  listAllDocumentsForCentroid(const std::string &) override;
  folly::Future<folly::Optional<std::vector<std::string>>>
  listAllDocumentsForCentroidOption(const std::string &) override;

  folly::Future<folly::Try<std::vector<std::string>>>
  listCentroidDocumentRangeFromOffset(const std::string &,
                                      size_t offset,
                                      size_t limit) override;
  folly::Future<folly::Optional<std::vector<std::string>>>
  listCentroidDocumentRangeFromOffsetOption(const std::string &,
                                            size_t offset,
                                            size_t limit) override;

  folly::Future<folly::Try<std::vector<std::string>>>
  listCentroidDocumentRangeFromDocumentId(const std::string &,
                                          const std::string &,
                                          size_t limit) override;
  folly::Future<folly::Optional<std::vector<std::string>>>
  listCentroidDocumentRangeFromDocumentIdOption(const std::string &,
                                                const std::string &,
                                                size_t limit) override;

  folly::Future<folly::Optional<std::string>> getCentroidMetadata(
      const std::string &, const std::string &) override;
  folly::Future<folly::Try<bool>> setCentroidMetadata(const std::string &,
                                                      const std::string &,
                                                      std::string) override;
  folly::Future<folly::Unit> debugEraseAllData() override;
};


} // persistence
} // relevanced
