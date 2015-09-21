#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>

#include "persistence/CentroidDB.h"
#include "persistence/CollectionDB.h"
#include "persistence/DocumentDB.h"
#include "CentroidUpdater.h"

namespace {
  using namespace std;
  using namespace folly;
  using namespace wangle;
}

class CentroidManager {
protected:
  persistence::CentroidDB *centroidDb_;
  persistence::CollectionDB *collectionDb_;
  persistence::DocumentDB *documentDb_;
  CentroidUpdater updater_;

  CentroidManager () {
    centroidDb_ = persistence::CentroidDB::getInstance();
    collectionDb_ = persistence::CollectionDB::getInstance();
    documentDb_ = persistence::DocumentDB::getInstance();
  }
  CentroidManager(CentroidManager const&) = delete;
  void operator=(CentroidManager const&) = delete;

public:
  static CentroidManager* getInstance() {
    static CentroidManager instance;
    return &instance;
  }

  Future<ProcessedCentroid*> getCentroid(const string &id) {
    return centroidDb_->doesCentroidExist(id).then([this, id] (bool doesExist) {
      if (doesExist) {
        return centroidDb_->loadCentroid(id);
      } else {
        return updater_.update(id).then([this, id](bool updated) {
          return centroidDb_->loadCentroid(id);
        });
      }
    });
  }

  Future<bool> update(const string &id) {
    return updater_.update(id);
  }

};

