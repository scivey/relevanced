#include <string>
#include <memory>
#include <vector>
#include <functional>
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
  : updater_(std::move(updater)), persistence_(std::move(persistence)) {
    updater_->onUpdate([this](const string &id) {
      this->echoUpdated(id);
    });
  }

Future<ProcessedCentroid*> CentroidManager::getCentroid(const string &id) {
  return persistence_->getCentroidDb().lock()->doesCentroidExist(id).then([this, id] (bool doesExist) {
    if (doesExist) {
      return persistence_->getCentroidDb().lock()->loadCentroid(id);
    } else {
      return updater_->update(id).then([this, id](bool updated) {
        return persistence_->getCentroidDb().lock()->loadCentroid(id);
      });
    }
  });
}

Future<bool> CentroidManager::update(const string &id) {
  return updater_->update(id);
}

void CentroidManager::echoUpdated(const string &collectionId) {
  SYNCHRONIZED(updateCallbacks_) {
    for (auto &cb: updateCallbacks_) {
      cb(collectionId);
    }
  }
}

void CentroidManager::triggerUpdate(const string &id) {
  updater_->triggerUpdate(id);
}

void CentroidManager::onUpdate(function<void (const string&)> fn) {
  updateCallbacks_->push_back(std::move(fn));
}
