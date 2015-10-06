#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <folly/futures/Try.h>
#include <folly/futures/helpers.h>

#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "persistence/Persistence.h"
#include "util/Debouncer.h"

namespace relevanced {
namespace centroid_update_worker {

using util::Debouncer;
using persistence::PersistenceIf;
using namespace std;
using namespace folly;
using namespace wangle;

CentroidUpdateWorker::CentroidUpdateWorker(
  shared_ptr<CentroidUpdaterFactoryIf> updaterFactory,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
): updaterFactory_(updaterFactory), threadPool_(threadPool) {}

void CentroidUpdateWorker::initialize() {
  chrono::milliseconds initialDelay(5000);
  chrono::milliseconds debounceInterval(30000);
  updateQueue_ = make_shared<Debouncer<string>>(
    initialDelay, debounceInterval, [this](string centroidId) {
      update(centroidId);
    }
  );
}

void CentroidUpdateWorker::stop() {
  stopping_ = true;
  updateQueue_->stop();
}

Future<Try<bool>> CentroidUpdateWorker::update(const string &centroidId) {
  return update(centroidId, chrono::milliseconds(50));
}

Future<Try<bool>> CentroidUpdateWorker::update(const string &centroidId, chrono::milliseconds updateDelay) {
  return threadPool_->addFuture([this, centroidId, updateDelay](){
    bool shouldUpdate = false;
    SYNCHRONIZED(updatingSet_) {
      if (updatingSet_.find(centroidId) == updatingSet_.end()) {
        shouldUpdate = true;
        updatingSet_.insert(centroidId);
      }
    }
    if (!shouldUpdate) {
      auto result = Try<bool>(false);
      return makeFuture(result);
    }
    auto updater = updaterFactory_->makeForCentroidId(centroidId);
    auto result = updater->run();
    return makeFuture(centroidId).delayed(updateDelay).then([this, result](string centroidId) {
      SYNCHRONIZED(updatingSet_) {
        updatingSet_.erase(centroidId);
      }
      if (!result.hasException()) {
        this->echoUpdated(centroidId);
      }
      return result;
    });
  });
}

void CentroidUpdateWorker::echoUpdated(const string &centroidId) {
  SYNCHRONIZED(updateCallbacks_) {
    for (auto &cb: updateCallbacks_) {
      cb(centroidId);
    }
  }
  vector<function<void(Try<string>)>> forCentroidCbs;
  SYNCHRONIZED(perCentroidUpdateCallbacks_) {
    auto callbacksPair = perCentroidUpdateCallbacks_.find(centroidId);
    if (callbacksPair != perCentroidUpdateCallbacks_.end()) {
      for (auto &elem: callbacksPair->second) {
        forCentroidCbs.push_back(elem);
      }
      callbacksPair->second.clear();
    }
  }
  for (auto &cb: forCentroidCbs) {
    cb(Try<string>(centroidId));
  }

}

void CentroidUpdateWorker::onUpdate(function<void (const string&)> callback) {
  updateCallbacks_->push_back(std::move(callback));
}

void CentroidUpdateWorker::onUpdateSpecificOnce(const string &id, function<void (Try<string>)> callback) {
  SYNCHRONIZED(perCentroidUpdateCallbacks_) {
    auto callbacksPair = perCentroidUpdateCallbacks_.find(id);
    if (callbacksPair == perCentroidUpdateCallbacks_.end()) {
      string tempId = id;
      perCentroidUpdateCallbacks_.insert(make_pair(tempId, vector<decltype(callback)> {}));
      callbacksPair = perCentroidUpdateCallbacks_.find(id);
    }
    callbacksPair->second.push_back(callback);
  }
}

folly::Future<Try<string>> CentroidUpdateWorker::joinUpdate(const string &id) {
  auto promise = make_shared<Promise<Try<string>>>();
  onUpdateSpecificOnce(id, [promise](Try<string> result) {
    promise->setValue(result);
  });
  update(id);
  return promise->getFuture();
}

void CentroidUpdateWorker::triggerUpdate(const string &centroidId) {
  string toWrite = centroidId;
  updateQueue_->write(toWrite);
}

} // centroid_update_worker
} // relevanced