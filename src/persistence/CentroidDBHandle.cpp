#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>

#include <folly/Format.h>
#include <folly/Optional.h>

#include <glog/logging.h>

#include "CentroidDBHandle.h"
#include "serialization/serializers.h"
#include "RockHandle.h"
#include "Centroid.h"
#include "util.h"
namespace persistence {


CentroidDBHandle::CentroidDBHandle(util::UniquePointer<RockHandleIf> rockHandle)
  : rockHandle_(std::move(rockHandle)) {}

bool CentroidDBHandle::doesCentroidExist(const string &id) {
  return rockHandle_->exists(id);
}

bool CentroidDBHandle::saveCentroid(const string &id, Centroid *centroid) {
  auto val = serialization::jsonSerialize<Centroid>(centroid);
  rockHandle_->put(id, val);
  return true;
}

bool CentroidDBHandle::deleteCentroid(const string &id) {
  return rockHandle_->del(id);
}

Optional<shared_ptr<Centroid>> CentroidDBHandle::loadCentroid(const string &id) {
  string serialized;
  Optional<shared_ptr<Centroid>> result;
  if (rockHandle_->get(id, serialized)) {
    auto centroidRes = new Centroid;
    auto loaded = serialization::jsonDeserialize<Centroid>(serialized);
    *centroidRes = loaded;
    result.assign(shared_ptr<Centroid>(centroidRes));
  }
  return result;
}


} // persistence
