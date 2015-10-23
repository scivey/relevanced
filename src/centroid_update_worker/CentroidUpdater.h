#pragma once
#include <vector>
#include <memory>
#include <string>
#include <folly/futures/Try.h>
#include "centroid_update_worker/DocumentAccumulatorFactory.h"
#include "declarations.h"

namespace relevanced {
namespace centroid_update_worker {

class CentroidUpdaterIf {
 public:
  virtual folly::Try<bool> run() = 0;
  virtual ~CentroidUpdaterIf() = default;
};

class CentroidUpdater : public CentroidUpdaterIf {
 protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  std::shared_ptr<persistence::CentroidMetadataDbIf> centroidMetadataDb_;
  std::shared_ptr<util::ClockIf> clock_;
  std::shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory_;
  std::string centroidId_;

 public:
  CentroidUpdater(std::shared_ptr<persistence::PersistenceIf>,
                  std::shared_ptr<persistence::CentroidMetadataDbIf>,
                  std::shared_ptr<util::ClockIf>,
                  std::shared_ptr<DocumentAccumulatorFactoryIf>,
                  std::string centroidId);
  folly::Try<bool> run() override;
};

} // centroid_update_worker
} // relevanced
