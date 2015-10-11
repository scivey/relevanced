#pragma once

#include <memory>
#include <string>
#include <folly/Optional.h>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>

#include "declarations.h"

namespace relevanced {
namespace persistence {

class CentroidMetadataDbIf {
 public:
  virtual folly::Future<folly::Optional<uint64_t>> getCreatedTimestamp(
      const std::string&) = 0;
  virtual folly::Future<folly::Optional<uint64_t>> getLastCalculatedTimestamp(
      const std::string&) = 0;
  virtual folly::Future<folly::Optional<uint64_t>>
  getLastDocumentChangeTimestamp(const std::string&) = 0;

  virtual folly::Future<folly::Try<bool>> isCentroidUpToDate(
      const std::string&) = 0;

  virtual folly::Future<folly::Try<bool>> setCreatedTimestamp(
      const std::string&, uint64_t) = 0;
  virtual folly::Future<folly::Try<bool>> setLastCalculatedTimestamp(
      const std::string&, uint64_t) = 0;
  virtual folly::Future<folly::Try<bool>> setLastDocumentChangeTimestamp(
      const std::string&, uint64_t) = 0;
  virtual ~CentroidMetadataDbIf() = default;
};

/**
 * `CentroidMetadataDb` exists to keep the typecasting of centroid
 *  metadata in one place.  It's just a wrapper around calls to
 *  `setCentroidMetadata` and `getCentroidMetadata`.
 */

class CentroidMetadataDb : public CentroidMetadataDbIf {
  std::shared_ptr<PersistenceIf> persistence_;

 public:
  CentroidMetadataDb(std::shared_ptr<PersistenceIf>);
  folly::Future<folly::Optional<uint64_t>> getCreatedTimestamp(
      const std::string&) override;
  folly::Future<folly::Optional<uint64_t>> getLastCalculatedTimestamp(
      const std::string&) override;
  folly::Future<folly::Optional<uint64_t>> getLastDocumentChangeTimestamp(
      const std::string&) override;

  folly::Future<folly::Try<bool>> isCentroidUpToDate(
      const std::string&) override;

  folly::Future<folly::Try<bool>> setCreatedTimestamp(const std::string&,
                                                      uint64_t) override;
  folly::Future<folly::Try<bool>> setLastCalculatedTimestamp(const std::string&,
                                                             uint64_t) override;
  folly::Future<folly::Try<bool>> setLastDocumentChangeTimestamp(
      const std::string&, uint64_t) override;
};


} // persistence
} // relevanced
