#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <folly/futures/Future.h>
#include <folly/Optional.h>

#include "persistence/PersistenceService.h"
#include "CentroidUpdater.h"
#include "CentroidManager.h"
#include "util.h"
using namespace std;
using namespace folly;
using namespace wangle;
using persistence::PersistenceServiceIf;
using util::UniquePointer;

CentroidManager::CentroidManager(UniquePointer<CentroidUpdaterIf> updater, shared_ptr<PersistenceServiceIf> persistence)
  : updater_(std::move(updater)), persistence_(std::move(persistence)) {
    updater_->onUpdate([this](const string &id) {
      this->echoUpdated(id);
    });
  }

Future<Optional<shared_ptr<ProcessedCentroid>>> CentroidManager::getCentroid(const string &id) {
  return persistence_->getCentroidDb().lock()->loadCentroid(id).then([this, id] (Optional<shared_ptr<ProcessedCentroid>> optCentroid) {
    if (optCentroid.hasValue()) {
      return optCentroid;
    } else {
      persistence_->getCollectionDb().lock()->doesCollectionExist(id).then([this, id](bool doesExist) {
        if (doesExist) {
          this->triggerUpdate(id);
        }
      });
      return optCentroid;
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
