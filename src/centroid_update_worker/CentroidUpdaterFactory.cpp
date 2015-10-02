#include <memory>
#include <string>
#include "persistence/Persistence.h"
#include "centroid_update_worker/CentroidUpdater.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"

namespace relevanced {
namespace centroid_update_worker {

CentroidUpdaterFactory::CentroidUpdaterFactory(shared_ptr<persistence::PersistenceIf> persistence)
  : persistence_(persistence) {}

shared_ptr<CentroidUpdaterIf> CentroidUpdaterFactory::makeForCentroidId(const string &centroidId) {
  return shared_ptr<CentroidUpdaterIf>(
    new CentroidUpdater(persistence_, centroidId)
  );
}

} // centroid_update_worker
} // relevanced
