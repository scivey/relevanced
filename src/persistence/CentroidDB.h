#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/Optional.h>

#include "CentroidDBHandle.h"
#include "RockHandle.h"
#include "Centroid.h"
#include "util.h"

namespace persistence {

class CentroidDBIf {
public:
  virtual folly::Future<bool> doesCentroidExist(const std::string&) = 0;
  virtual folly::Future<bool> deleteCentroid(const std::string&) = 0;
  virtual folly::Future<bool> saveCentroid(const std::string&, Centroid*) = 0;
  virtual folly::Future<bool> saveCentroid(const std::string&, std::shared_ptr<Centroid>) = 0;
  virtual folly::Future<folly::Optional<std::shared_ptr<Centroid>>>
    loadCentroid(const std::string&) = 0;
};

class CentroidDB: public CentroidDBIf {
protected:
  util::UniquePointer<CentroidDBHandleIf> dbHandle_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;

  CentroidDB(CentroidDB const&) = delete;
  void operator=(CentroidDB const&) = delete;

public:
  CentroidDB(
    util::UniquePointer<CentroidDBHandleIf> dbHandle,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
  );

  folly::Future<bool> doesCentroidExist(const std::string &id) override;
  folly::Future<bool> deleteCentroid(const std::string &id) override;
  folly::Future<bool> saveCentroid(const std::string &id, Centroid *centroid) override;
  folly::Future<bool> saveCentroid(const std::string &id, std::shared_ptr<Centroid> centroid) override;
  folly::Future<folly::Optional<std::shared_ptr<Centroid>>>
    loadCentroid(const std::string &id) override;
};

} // persistence
