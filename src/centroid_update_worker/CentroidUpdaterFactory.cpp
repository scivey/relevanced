#include <memory>
#include <string>
#include "persistence/Persistence.h"
#include "centroid_update_worker/CentroidUpdater.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "util/Clock.h"

using namespace std;

namespace relevanced {
namespace centroid_update_worker {

CentroidUpdaterFactory::CentroidUpdaterFactory(
    shared_ptr<persistence::PersistenceIf> persistence,
    shared_ptr<persistence::CentroidMetadataDbIf> metadata,
    shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory,
    shared_ptr<util::ClockIf> clock)
    : persistence_(persistence),
      centroidMetadataDb_(metadata),
      accumulatorFactory_(accumulatorFactory),
      clock_(clock) {}

shared_ptr<CentroidUpdaterIf> CentroidUpdaterFactory::makeForCentroidId(
    const string &centroidId) {
  return shared_ptr<CentroidUpdaterIf>(new CentroidUpdater(
      persistence_, centroidMetadataDb_, clock_, accumulatorFactory_, centroidId));
}

} // centroid_update_worker
} // relevanced
