#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "CentroidDBHandle.h"
#include "RockHandle.h"
#include "ProcessedCentroid.h"
#include "util.h"

namespace persistence {

class CentroidDBIf {
public:
  virtual folly::Future<bool> doesCentroidExist(const std::string&) = 0;
  virtual folly::Future<bool> deleteCentroid(const std::string&) = 0;
  virtual folly::Future<bool> saveCentroid(const std::string&, ProcessedCentroid*) = 0;
  virtual folly::Future<bool> saveCentroid(const std::string&, std::shared_ptr<ProcessedCentroid>) = 0;
  virtual folly::Future<ProcessedCentroid*> loadCentroid(const std::string&) = 0;
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
  folly::Future<bool> saveCentroid(const std::string &id, ProcessedCentroid *centroid) override;
  folly::Future<bool> saveCentroid(const std::string &id, std::shared_ptr<ProcessedCentroid> centroid) override;
  folly::Future<ProcessedCentroid*> loadCentroid(const std::string &id) override;
};

} // persistence
