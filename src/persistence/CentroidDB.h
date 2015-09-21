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


namespace {
  using namespace std;
  using namespace folly;
  using namespace wangle;
}

namespace persistence {

class CentroidDB {
protected:
  CentroidDBHandle *dbHandle_ {nullptr};
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {1};
  CentroidDB () {
    threadPool_.addFuture([this](){
      auto rock = std::make_unique<RockHandle>("data/centroids");
      dbHandle_ = new CentroidDBHandle(std::move(rock));
    });
  }
  CentroidDB(CentroidDB const&) = delete;
  void operator=(CentroidDB const&) = delete;

public:

  static CentroidDB* getInstance() {
    static CentroidDB instance;
    return &instance;
  }

  Future<bool> doesCentroidExist(const string &id) {
    return threadPool_.addFuture([this, id](){
      return dbHandle_->doesCentroidExist(id);
    });
  }

  Future<bool> deleteCentroid(const string &id) {
    return threadPool_.addFuture([this, id](){
      return dbHandle_->deleteCentroid(id);
    });
  }

  Future<bool> saveCentroid(const string &id, ProcessedCentroid *centroid) {
    return threadPool_.addFuture([this, id, centroid](){
      return dbHandle_->saveCentroid(id, centroid);
    });
  }

  Future<ProcessedCentroid*> loadCentroid(const string &id) {
    return threadPool_.addFuture([this, id](){
      return dbHandle_->loadCentroid(id);
    });
  }

};

} // persistence
