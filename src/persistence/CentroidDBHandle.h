#pragma once
#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <folly/Optional.h>
#include <glog/logging.h>
#include "RockHandle.h"
#include "ProcessedCentroid.h"
#include "util.h"

namespace persistence {

class CentroidDBHandleIf {
public:
  virtual bool doesCentroidExist(const std::string &id) = 0;
  virtual bool saveCentroid(const std::string &id, ProcessedCentroid *centroid) = 0;
  virtual bool deleteCentroid(const std::string &id) = 0;
  virtual folly::Optional<std::shared_ptr<ProcessedCentroid>>
    loadCentroid(const std::string &id) = 0;
  virtual ~CentroidDBHandleIf() = default;
};

class CentroidDBHandle: public CentroidDBHandleIf {
protected:
  util::UniquePointer<RockHandleIf> rockHandle_;
public:
  CentroidDBHandle(util::UniquePointer<RockHandleIf> rockHandle);
  bool doesCentroidExist(const std::string &id) override;
  bool saveCentroid(const std::string &id, ProcessedCentroid *centroid) override;
  bool deleteCentroid(const std::string &id) override;
  folly::Optional<std::shared_ptr<ProcessedCentroid>>
    loadCentroid(const std::string &id) override;
};

} // persistence
