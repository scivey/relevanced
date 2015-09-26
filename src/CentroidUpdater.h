#pragma once

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/io/async/EventBase.h>


#include "DebouncedQueue.h"
#include "CentroidUpdateWorker.h"
#include "persistence/PersistenceService.h"

class CentroidUpdaterIf {
public:
  virtual void initialize() = 0;
  virtual void echoUpdated(const std::string&) = 0;
  virtual void onUpdate(std::function<void (const std::string&)>) = 0;
  virtual void triggerUpdate(const std::string &collectionId) = 0;
  virtual folly::Future<bool> update(const std::string &collectionId) = 0;
  virtual ~CentroidUpdaterIf() = default;
};

class CentroidUpdater: public CentroidUpdaterIf {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
  std::shared_ptr<DebouncedQueue<std::string>> updateQueue_;
  std::shared_ptr<std::thread> dequeueThread_;
  std::shared_ptr<std::thread> evThread_;
  folly::EventBase base_;
  folly::Synchronized<std::vector<std::function<void(const std::string&)>>> updateCallbacks_;
public:
  CentroidUpdater(
    std::shared_ptr<persistence::PersistenceServiceIf>,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
  );
  void initialize() override;
  void echoUpdated(const std::string&) override;
  void onUpdate(std::function<void (const std::string&)>) override;
  void triggerUpdate(const std::string &collectionId) override;
  folly::Future<bool> update(const std::string &collectionId) override;
};

