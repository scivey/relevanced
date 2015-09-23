#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "CentroidUpdater.h"
#include "CentroidUpdateWorker.h"
#include "persistence/PersistenceService.h"

using persistence::PersistenceServiceIf;
using namespace std;
using namespace folly;
using namespace wangle;

CentroidUpdater::CentroidUpdater(
  shared_ptr<PersistenceServiceIf> persistence,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
): persistence_(persistence), threadPool_(threadPool) {}

Future<bool> CentroidUpdater::update(const string &collectionId) {
  return threadPool_.addFuture([this, collectionId](){
    CentroidUpdateWorker worker(persistence_, collectionId);
    return worker.run();
  });
}

