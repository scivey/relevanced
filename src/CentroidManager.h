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

class CentroidManager {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  util::UniquePointer<CentroidUpdater> updater_;
  CentroidManager(CentroidManager const&) = delete;
  void operator=(CentroidManager const&) = delete;
  folly::Synchronized<std::vector<std::function<void(const std::string&)>>> updateCallbacks_;
public:
  CentroidManager(
    util::UniquePointer<CentroidUpdater>,
    std::shared_ptr<persistence::PersistenceServiceIf>
  );
  folly::Future<folly::Optional<std::shared_ptr<ProcessedCentroid>>>
    getCentroid(const std::string &id);
  void onUpdate(std::function<void (const std::string&)>);
  void echoUpdated(const std::string&);
  folly::Future<bool> update(const std::string &id);
  void triggerUpdate(const std::string &id);
};

