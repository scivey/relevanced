#pragma once
#include <memory>
#include "util/util.h"
#include "centroid_update_worker/DocumentAccumulator.h"

namespace relevanced {
namespace centroid_update_worker {

class DocumentAccumulatorFactoryIf {
public:
  virtual util::UniquePointer<DocumentAccumulatorIf> get() = 0;
  virtual ~DocumentAccumulatorFactoryIf() = default;
};

class DocumentAccumulatorFactory: public DocumentAccumulatorFactoryIf {
public:
  util::UniquePointer<DocumentAccumulatorIf> get() override;
};

} // centroid_update_worker
} // relevanced
