#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include "CentroidUpdater.h"
#include "CentroidUpdateWorker.h"
#include "persistence/Persistence.h"

using persistence::PersistenceIf;
using namespace std;
using namespace folly;
using namespace wangle;

CentroidUpdater::CentroidUpdater(
  shared_ptr<PersistenceIf> persistence,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
): persistence_(persistence), threadPool_(threadPool) {}

void CentroidUpdater::initialize() {
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

Future<bool> CentroidUpdater::update(const string &classifierId) {
  return threadPool_->addFuture([this, classifierId](){
    CentroidUpdateWorker worker(persistence_, classifierId);
    bool result = worker.run();
    makeFuture(classifierId).delayed(chrono::milliseconds(50)).then([this](string collId) {
      this->echoUpdated(collId);
    });
    return result;
  });
}

void CentroidUpdater::echoUpdated(const string &classifierId) {
  SYNCHRONIZED(updateCallbacks_) {
    for (auto &cb: updateCallbacks_) {
      cb(classifierId);
    }
  }
}

void CentroidUpdater::onUpdate(function<void (const string&)> callback) {
  updateCallbacks_->push_back(std::move(callback));
}

void CentroidUpdater::triggerUpdate(const string &classifierId) {
  updateQueue_->write(classifierId);
}
