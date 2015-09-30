#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/io/async/EventBase.h>

#include "CentroidUpdateWorker.h"
#include "CentroidUpdater.h"
#include "CentroidUpdaterFactory.h"

#include "Debouncer.h"
#include "persistence/Persistence.h"

class CentroidUpdateWorkerIf {
public:
  virtual void initialize() = 0;
  virtual void echoUpdated(const std::string&) = 0;
  virtual void onUpdate(std::function<void (const std::string&)>) = 0;
  virtual void triggerUpdate(const std::string &centroidId) = 0;
  virtual folly::Future<bool> update(const std::string &centroidId) = 0;
  virtual folly::Future<bool> update(const std::string &centroidId, std::chrono::milliseconds updateDelay) = 0;
  virtual ~CentroidUpdateWorkerIf() = default;
};

class CentroidUpdateWorker: public CentroidUpdateWorkerIf {
protected:
  std::shared_ptr<CentroidUpdaterFactoryIf> updaterFactory_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
  std::shared_ptr<Debouncer<std::string>> updateQueue_;
  folly::Synchronized<std::vector<std::function<void(const std::string&)>>> updateCallbacks_;
  std::atomic<bool> stopping_ {false};
public:
  CentroidUpdateWorker(
    std::shared_ptr<CentroidUpdaterFactoryIf> updaterFactory,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
  );
  void stop();
  void initialize() override;
  void echoUpdated(const std::string&) override;
  void onUpdate(std::function<void (const std::string&)>) override;
  void triggerUpdate(const std::string &centroidId) override;
  folly::Future<bool> update(const std::string &centroidId) override;
  folly::Future<bool> update(const std::string &centroidId, std::chrono::milliseconds updateDelay) override;
};

