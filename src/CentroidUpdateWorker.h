#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>
#include "util.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "CentroidFactory.h"
#include "persistence/PersistenceService.h"

class CentroidUpdateWorker {
protected:
  std::shared_ptr<persistence::PersistenceService> persistence_;
  string collectionId_;
public:
  CentroidUpdateWorker(
    std::shared_ptr<persistence::PersistenceService>,
    std::string collectionId
  );
  bool run();
};
