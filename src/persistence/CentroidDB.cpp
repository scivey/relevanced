#include "CentroidDB.h"
#include "util.h"
#include <memory>
#include <string>
#include <folly/futures/Future.h>
#include <folly/Optional.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
using namespace std;
using namespace folly;
using namespace wangle;;

namespace persistence {

CentroidDB::CentroidDB(
  util::UniquePointer<CentroidDBHandleIf> dbHandle,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
) : dbHandle_(std::move(dbHandle)), threadPool_(threadPool) {}

Future<bool> CentroidDB::doesCentroidExist(const string &id) {
  return threadPool_->addFuture([this, id](){
    return dbHandle_->doesCentroidExist(id);
  });
}

Future<bool> CentroidDB::deleteCentroid(const string &id) {
  return threadPool_->addFuture([this, id](){
    return dbHandle_->deleteCentroid(id);
  });
}

Future<bool> CentroidDB::saveCentroid(const string &id, Centroid *centroid) {
  return threadPool_->addFuture([this, id, centroid](){
    return dbHandle_->saveCentroid(id, centroid);
  });
}

Future<bool> CentroidDB::saveCentroid(const string &id, shared_ptr<Centroid> centroid) {
  return threadPool_->addFuture([this, id, centroid](){
    return dbHandle_->saveCentroid(id, centroid.get());
  });
}

Future<Optional<shared_ptr<Centroid>>> CentroidDB::loadCentroid(const string &id) {
  return threadPool_->addFuture([this, id](){
    return dbHandle_->loadCentroid(id);
  });
}

} // persistence
