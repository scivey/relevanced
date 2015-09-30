#pragma once
#include <vector>
#include <memory>
#include <string>
#include "persistence/Persistence.h"

class CentroidUpdater {
protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  string centroidId_;
public:
  CentroidUpdater(
    std::shared_ptr<persistence::PersistenceIf>,
    std::string centroidId
  );
  bool run();
};

