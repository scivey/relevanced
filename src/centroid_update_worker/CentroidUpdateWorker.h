#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/io/async/EventBase.h>

#include "declarations.h"

#include "util/Debouncer.h"

namespace relevanced {
namespace centroid_update_worker {

class CentroidUpdateWorkerIf {
 public:
  virtual void initialize() = 0;
  virtual void echoUpdated(const std::string &) = 0;
  virtual folly::Future<folly::Try<std::string>> joinUpdate(
      const std::string &) = 0;
  virtual void onUpdate(std::function<void(const std::string &) >) = 0;
  virtual void onUpdateSpecificOnce(
      const std::string &id, std::function<void(folly::Try<std::string>) >) = 0;
  virtual void triggerUpdate(const std::string &centroidId) = 0;
  virtual folly::Future<folly::Try<bool>> update(
      const std::string &centroidId) = 0;
  virtual folly::Future<folly::Try<bool>> update(
      const std::string &centroidId, std::chrono::milliseconds updateDelay) = 0;
  virtual void stop() = 0;
  virtual void join() = 0;
  virtual ~CentroidUpdateWorkerIf() = default;
};

class CentroidUpdateWorker : public CentroidUpdateWorkerIf {
 protected:
  std::shared_ptr<CentroidUpdaterFactoryIf> updaterFactory_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
      threadPool_;
  std::shared_ptr<util::Debouncer<std::string>> updateQueue_;
  folly::Synchronized<std::vector<std::function<void(const std::string &) >>>
      updateCallbacks_;
  folly::Synchronized<
      std::map<std::string,
               std::vector<std::function<void(folly::Try<std::string>) >>>>
      perCentroidUpdateCallbacks_;
  folly::Synchronized<std::set<std::string>> updatingSet_;
  std::atomic<bool> stopping_{false};
  std::atomic<size_t> numInProgress_ {0};
  void incrInProgress();
  void decrInProgress();

 public:
  CentroidUpdateWorker(
      std::shared_ptr<CentroidUpdaterFactoryIf> updaterFactory,
      std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>);
  void stop() override;
  void join() override;
  void initialize() override;
  void echoUpdated(const std::string &) override;
  folly::Future<folly::Try<std::string>> joinUpdate(
      const std::string &) override;
  void onUpdate(std::function<void(const std::string &) >) override;
  void onUpdateSpecificOnce(
      const std::string &id,
      std::function<void(folly::Try<std::string>) >) override;
  void triggerUpdate(const std::string &centroidId) override;
  folly::Future<folly::Try<bool>> update(
      const std::string &centroidId) override;
  folly::Future<folly::Try<bool>> update(
      const std::string &centroidId,
      std::chrono::milliseconds updateDelay) override;

  std::shared_ptr<util::Debouncer<std::string>> debug_getUpdateQueue();

  ~CentroidUpdateWorker();
};

} // centroid_update_worker
} // relevanced