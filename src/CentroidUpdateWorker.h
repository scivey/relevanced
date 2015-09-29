#pragma once
#include <vector>
#include <memory>
#include <string>
#include "persistence/Persistence.h"

class CentroidUpdateWorker {
protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  string centroidId_;
public:
  CentroidUpdateWorker(
    std::shared_ptr<persistence::PersistenceIf>,
    std::string centroidId
  );
  bool run();
};
