#include <memory>
#include "util/util.h"
#include "centroid_update_worker/DocumentAccumulatorFactory.h"
#include "centroid_update_worker/DocumentAccumulator.h"

namespace relevanced {
namespace centroid_update_worker {

using util::UniquePointer;

UniquePointer<DocumentAccumulatorIf> DocumentAccumulatorFactory::get() {
  return UniquePointer<DocumentAccumulatorIf>(new DocumentAccumulator);
}

} // centroid_update_worker
} // relevanced