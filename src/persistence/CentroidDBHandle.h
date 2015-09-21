#pragma once
#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>
#include "RockHandle.h"
#include "ProcessedCentroid.h"

namespace {
  using namespace std;
  using namespace folly;
}
namespace persistence {

class CentroidDBHandle {
protected:
  unique_ptr<RockHandle> rockHandle_;
public:
  CentroidDBHandle(unique_ptr<RockHandle> rockHandle)
    : rockHandle_(std::move(rockHandle)) {}

  bool doesCentroidExist(const string &id) {
    return rockHandle_->exists(id);
  }

  bool saveCentroid(const string &id, ProcessedCentroid *centroid) {
    auto val = centroid->toJson();
    rockHandle_->put(id, val);
    return true;
  }

  bool deleteCentroid(const string &id) {
    return rockHandle_->del(id);
  }

  ProcessedCentroid* loadCentroid(const string &id) {
    auto serialized = rockHandle_->get(id);
    return ProcessedCentroid::newFromJson(serialized);
  }

};

} // persistence
