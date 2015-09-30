#pragma once
#include <vector>
#include <memory>
#include <string>
#include "persistence/Persistence.h"
#include "CentroidUpdater.h"
#include "util.h"

class CentroidUpdaterFactoryIf {
public:
  virtual std::shared_ptr<CentroidUpdaterIf> makeForCentroidId(const std::string&) = 0;
  virtual ~CentroidUpdaterFactoryIf() = default;
};

class CentroidUpdaterFactory: public CentroidUpdaterFactoryIf {
protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
public:
  CentroidUpdaterFactory(
    std::shared_ptr<persistence::PersistenceIf>
  );
  std::shared_ptr<CentroidUpdaterIf> makeForCentroidId(const std::string&) override;
};

