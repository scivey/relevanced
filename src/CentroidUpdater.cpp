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
#include "persistence/PersistenceService.h"
using persistence::PersistenceServiceIf;
using namespace std;
using namespace folly;
using namespace wangle;

CentroidUpdater::CentroidUpdater(
  shared_ptr<PersistenceServiceIf> persistence,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
): persistence_(persistence), threadPool_(threadPool) {
  evThread_ = make_shared<thread>([this](){
    base_.loopForever();
  });
  updateQueue_ = make_shared<DebouncedQueue<string>>(
    &base_, 100, chrono::milliseconds(30000)
  );
  dequeueThread_ = make_shared<thread>([this](){
    string elem;
    for (;;) {
      this_thread::sleep_for(chrono::milliseconds(1000));
      while (updateQueue_->read(elem)) {
        LOG(INFO) << "background update: " << elem;
        update(elem);
      }
    }
  });
}

Future<bool> CentroidUpdater::update(const string &collectionId) {
  return threadPool_->addFuture([this, collectionId](){
    CentroidUpdateWorker worker(persistence_, collectionId);
    bool result = worker.run();
    makeFuture(collectionId).delayed(chrono::milliseconds(10)).then([this](string collId) {
      this->echoUpdated(collId);
    });
    return result;
  });
}

void CentroidUpdater::echoUpdated(const string &collectionId) {
  SYNCHRONIZED(updateCallbacks_) {
    for (auto &cb: updateCallbacks_) {
      cb(collectionId);
    }
  }
}

void CentroidUpdater::onUpdate(function<void (const string&)> callback) {
  updateCallbacks_->push_back(std::move(callback));
}

void CentroidUpdater::triggerUpdate(const string &collectionId) {
  updateQueue_->write(collectionId);
}
