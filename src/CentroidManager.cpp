#pragma once

#include <string>
#include <memory>
#include <vector>

#include <folly/futures/Future.h>

#include "persistence/PersistenceService.h"
#include "CentroidUpdater.h"
#include "CentroidManager.h"
#include "util.h"
using namespace std;
using namespace folly;
using namespace wangle;
using persistence::PersistenceServiceIf;
using util::UniquePointer;

CentroidManager::CentroidManager(UniquePointer<CentroidUpdater> updater, shared_ptr<PersistenceServiceIf> persistence)
  : updater_(std::move(updater)), persistence_(std::move(persistence)) {}

Future<ProcessedCentroid*> CentroidManager::getCentroid(const string &id) {
  return persistence_->getCentroidDb().lock()->doesCentroidExist(id).then([this, id] (bool doesExist) {
    if (doesExist) {
      return persistence_->getCentroidDb().lock()->loadCentroid(id);
    } else {
      return updater_.update(id).then([this, id](bool updated) {
        return persistence_->getCentroidDb().lock()->loadCentroid(id);
      });
    }
  });
}

Future<bool> CentroidManager::update(const string &id) {
  return updater_->update(id);
}
