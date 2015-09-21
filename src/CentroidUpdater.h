#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "CentroidUpdateWorker.h"

namespace {
  using namespace std;
  using namespace folly;
  using namespace wangle;
}

class CentroidUpdater {
protected:
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {2};
public:
  CentroidUpdater(){}
  Future<bool> update(const string &collectionId) {
    return threadPool_.addFuture([this, collectionId](){
      CentroidUpdateWorker worker(collectionId);
      return worker.run();
    });
  }
};

