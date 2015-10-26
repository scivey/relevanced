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

#include "centroid_update_worker/CentroidUpdater.h"
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
    shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
    : updaterFactory_(updaterFactory),
      threadPool_(threadPool) {}

void CentroidUpdateWorker::initialize() {
  chrono::milliseconds initialDelay(5000);
  chrono::milliseconds debounceInterval(30000);
  updateQueue_ = make_shared<Debouncer<string>>(
      initialDelay, debounceInterval,
      [this](string centroidId) { update(centroidId); });
}

void CentroidUpdateWorker::incrInProgress() {
  numInProgress_.fetch_add(1);
}

void CentroidUpdateWorker::decrInProgress() {
  makeFuture()
    .delayed(chrono::milliseconds(10))
    .then([this](){
      numInProgress_.fetch_sub(1);
    });
}

void CentroidUpdateWorker::stop() {
  if (!stopping_) {
    stopping_ = true;
    updateQueue_->stop();
  }
}

Future<Try<bool>> CentroidUpdateWorker::update(const string &centroidId) {
  if (stopping_) {
    Try<bool> response {false};
    return makeFuture(response);
  }
  return update(centroidId, chrono::milliseconds(50));
}

Future<Try<bool>> CentroidUpdateWorker::update(
    const string &centroidId, chrono::milliseconds updateDelay) {
  return threadPool_->addFuture([this, centroidId, updateDelay]() {
    bool shouldUpdate = false;
    SYNCHRONIZED(updatingSet_) {
      if (updatingSet_.find(centroidId) == updatingSet_.end()) {
        shouldUpdate = true;
        updatingSet_.insert(centroidId);
      }
    }
    if (!shouldUpdate || stopping_) {
      auto result = Try<bool>(false);
      return makeFuture(result);
    }
    incrInProgress();
    auto updater = updaterFactory_->makeForCentroidId(centroidId);
    auto result = updater->run();
    if (stopping_) {
      decrInProgress();
      if (result.hasException()) {
        return makeFuture<Try<bool>>(result.exception());
      } else {
        Try<bool> response(true);
        return makeFuture(response);
      }
    }
    return makeFuture(centroidId)
        .delayed(updateDelay)
        .then([this, result](string centroidId) {
          SYNCHRONIZED(updatingSet_) { updatingSet_.erase(centroidId); }
          if (!result.hasException()) {
            this->echoUpdated(centroidId);
          }
          decrInProgress();
          return result;
        });
  });
}

void CentroidUpdateWorker::echoUpdated(const string &centroidId) {
  SYNCHRONIZED(updateCallbacks_) {
    for (auto &cb : updateCallbacks_) {
      cb(centroidId);
    }
  }
  vector<function<void(Try<string>) >> forCentroidCbs;
  SYNCHRONIZED(perCentroidUpdateCallbacks_) {
    auto callbacksPair = perCentroidUpdateCallbacks_.find(centroidId);
    if (callbacksPair != perCentroidUpdateCallbacks_.end()) {
      for (auto &elem : callbacksPair->second) {
        forCentroidCbs.push_back(elem);
      }
      callbacksPair->second.clear();
    }
  }
  for (auto &cb : forCentroidCbs) {
    if (stopping_) {
      break;
    }
    cb(Try<string>(centroidId));
  }
}

void CentroidUpdateWorker::onUpdate(function<void(const string &) > callback) {
  updateCallbacks_->push_back(std::move(callback));
}

void CentroidUpdateWorker::onUpdateSpecificOnce(
    const string &id, function<void(Try<string>) > callback) {
  SYNCHRONIZED(perCentroidUpdateCallbacks_) {
    auto callbacksPair = perCentroidUpdateCallbacks_.find(id);
    if (callbacksPair == perCentroidUpdateCallbacks_.end()) {
      string tempId = id;
      perCentroidUpdateCallbacks_.insert(
          make_pair(tempId, vector<decltype(callback)>{}));
      callbacksPair = perCentroidUpdateCallbacks_.find(id);
    }
    callbacksPair->second.push_back(callback);
  }
}

folly::Future<Try<string>> CentroidUpdateWorker::joinUpdate(const string &id) {
  auto promise = make_shared<Promise<Try<string>>>();
  onUpdateSpecificOnce(
      id, [promise](Try<string> result) { promise->setValue(result); });
  update(id);
  return promise->getFuture();
}

void CentroidUpdateWorker::triggerUpdate(const string &centroidId) {
  string toWrite = centroidId;
  if (!stopping_) {
    updateQueue_->write(toWrite);
  }
}

// this is a pretty naive approach,
// but this object is only created/destroyed
// repeatedly in the tests.
void CentroidUpdateWorker::join() {
  stop();
  updateQueue_->join();
  for (;;) {
    auto inProgress = numInProgress_.load();
    if (inProgress == 0) {
      break;
    }
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}

CentroidUpdateWorker::~CentroidUpdateWorker() {}

shared_ptr<Debouncer<string>> CentroidUpdateWorker::debug_getUpdateQueue() {
  if (updateQueue_.get() == nullptr) {
    LOG(INFO) << "CentroidUpdateWorker has not been initialized!";
  }
  return updateQueue_;
}

} // centroid_update_worker
} // relevanced