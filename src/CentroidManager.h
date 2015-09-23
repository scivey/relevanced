#pragma once

#include <string>
#include <memory>
#include <folly/futures/Future.h>
#include "persistence/PersistenceService.h"
#include "CentroidUpdater.h"
#include "util.h"

class CentroidManager {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  util::UniquePointer<CentroidUpdater> updater_;
  CentroidManager(CentroidManager const&) = delete;
  void operator=(CentroidManager const&) = delete;
public:
  CentroidManager(
    util::UniquePointer<CentroidUpdater>,
    std::shared_ptr<persistence::PersistenceServiceIf>
  );
  folly::Future<ProcessedCentroid*> getCentroid(const std::string &id);
  folly::Future<bool> update(const std::string &id);
};

