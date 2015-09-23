#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>

#include <folly/Format.h>
#include <glog/logging.h>

#include "CentroidDBHandle.h"
#include "RockHandle.h"
#include "ProcessedCentroid.h"
#include "util.h"
namespace persistence {

CentroidDBHandle::CentroidDBHandle(UniquePointer<RockHandleIf> rockHandle)
  : rockHandle_(std::move(rockHandle)) {}

bool CentroidDBHandle::doesCentroidExist(const string &id) {
  return rockHandle_->exists(id);
}

bool CentroidDBHandle::saveCentroid(const string &id, ProcessedCentroid *centroid) {
  auto val = centroid->toJson();
  rockHandle_->put(id, val);
  return true;
}

bool CentroidDBHandle::deleteCentroid(const string &id) {
  return rockHandle_->del(id);
}

ProcessedCentroid* CentroidDBHandle::loadCentroid(const string &id) {
  auto serialized = rockHandle_->get(id);
  return ProcessedCentroid::newFromJson(serialized);
}

} // persistence
