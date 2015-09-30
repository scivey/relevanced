#include <memory>
#include <string>
#include "persistence/Persistence.h"
#include "CentroidUpdater.h"
#include "CentroidUpdaterFactory.h"

CentroidUpdaterFactory::CentroidUpdaterFactory(shared_ptr<persistence::PersistenceIf> persistence)
  : persistence_(persistence) {}

shared_ptr<CentroidUpdaterIf> CentroidUpdaterFactory::makeForCentroidId(const string &centroidId) {
  return shared_ptr<CentroidUpdaterIf>(
    new CentroidUpdater(persistence_, centroidId)
  );
}
