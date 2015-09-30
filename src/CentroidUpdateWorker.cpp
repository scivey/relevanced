#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include "CentroidUpdateWorker.h"
#include "persistence/Persistence.h"

using persistence::PersistenceIf;
using namespace std;
using namespace folly;
using namespace wangle;

CentroidUpdateWorker::CentroidUpdateWorker(
  shared_ptr<PersistenceIf> persistence,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
): persistence_(persistence), threadPool_(threadPool) {}

void CentroidUpdateWorker::initialize() {
  evThread_ = make_shared<thread>([this](){
    base_.loopForever();
  });
  chrono::milliseconds initialDelay(5000);
  chrono::milliseconds debounceInterval(30000);
  updateQueue_ = make_shared<DebouncedQueue<string>>(
    &base_, 100, initialDelay, debounceInterval
  );
  dequeueThread_ = make_shared<thread>([this](){
    string elem;
    for (;;) {
      this_thread::sleep_for(chrono::milliseconds(1000));
      while (updateQueue_->read(elem)) {
        update(elem);
      }
    }
  });
}

Future<bool> CentroidUpdateWorker::update(const string &centroidId) {
  return threadPool_->addFuture([this, centroidId](){
    CentroidUpdater updater(persistence_, centroidId);
    bool result = updater.run();
    makeFuture(centroidId).delayed(chrono::milliseconds(50)).then([this](string centroidId) {
      this->echoUpdated(centroidId);
    });
    return result;
  });
}

void CentroidUpdateWorker::echoUpdated(const string &centroidId) {
  SYNCHRONIZED(updateCallbacks_) {
    for (auto &cb: updateCallbacks_) {
      cb(centroidId);
    }
  }
}

void CentroidUpdateWorker::onUpdate(function<void (const string&)> callback) {
  updateCallbacks_->push_back(std::move(callback));
}

void CentroidUpdateWorker::triggerUpdate(const string &centroidId) {
  updateQueue_->write(centroidId);
}
