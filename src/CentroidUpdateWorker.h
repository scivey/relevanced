#pragma once
#include <vector>
#include <memory>
#include <string>
#include "persistence/PersistenceService.h"

class CentroidUpdateWorker {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  string classifierId_;
public:
  CentroidUpdateWorker(
    std::shared_ptr<persistence::PersistenceServiceIf>,
    std::string classifierId
  );
  bool run();
};
