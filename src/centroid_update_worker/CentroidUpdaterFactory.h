#pragma once
#include <vector>
#include <memory>
#include <string>
#include "centroid_update_worker/DocumentAccumulatorFactory.h"
#include "declarations.h"

namespace relevanced {
namespace centroid_update_worker {

class CentroidUpdaterFactoryIf {
 public:
  virtual std::shared_ptr<CentroidUpdaterIf>
    makeForCentroidId(const std::string&) = 0;

  virtual ~CentroidUpdaterFactoryIf() = default;
};

class CentroidUpdaterFactory : public CentroidUpdaterFactoryIf {
 protected:
  std::shared_ptr<persistence::PersistenceIf> persistence_;
  std::shared_ptr<persistence::CentroidMetadataDbIf> centroidMetadataDb_;
  std::shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory_;
  std::shared_ptr<util::ClockIf> clock_;

 public:
  CentroidUpdaterFactory(std::shared_ptr<persistence::PersistenceIf>,
                         std::shared_ptr<persistence::CentroidMetadataDbIf>,
                         std::shared_ptr<DocumentAccumulatorFactoryIf>,
                         std::shared_ptr<util::ClockIf>);

  std::shared_ptr<CentroidUpdaterIf>
    makeForCentroidId(const std::string&) override;
};

} // centroid_update_worker
} // relevanced
