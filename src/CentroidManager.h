#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <folly/futures/Future.h>
#include <folly/Optional.h>
#include <folly/Synchronized.h>
#include "persistence/PersistenceService.h"
#include "CentroidUpdater.h"
#include "util.h"

class CentroidManagerIf {
public:
  virtual folly::Future<folly::Optional<std::shared_ptr<ProcessedCentroid>>>
    getCentroid(const std::string &id) = 0;
  virtual void onUpdate(std::function<void (const std::string&)>) = 0;
  virtual void echoUpdated(const std::string&) = 0;
  virtual folly::Future<bool> update(const std::string &id) = 0;
  virtual void triggerUpdate(const std::string &id) = 0;
  virtual ~CentroidManagerIf() = default;
};

class CentroidManager: public CentroidManagerIf {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  util::UniquePointer<CentroidUpdaterIf> updater_;
  CentroidManager(CentroidManager const&) = delete;
  void operator=(CentroidManager const&) = delete;
  folly::Synchronized<std::vector<std::function<void(const std::string&)>>> updateCallbacks_;
public:
  CentroidManager(
    util::UniquePointer<CentroidUpdaterIf>,
    std::shared_ptr<persistence::PersistenceServiceIf>
  );
  folly::Future<folly::Optional<std::shared_ptr<ProcessedCentroid>>>
    getCentroid(const std::string &id) override;
  void onUpdate(std::function<void (const std::string&)>) override;
  void echoUpdated(const std::string&) override;
  folly::Future<bool> update(const std::string &id) override;
  void triggerUpdate(const std::string &id) override;
};

