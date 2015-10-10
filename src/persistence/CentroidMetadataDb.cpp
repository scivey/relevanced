#include "persistence/CentroidMetadataDb.h"
#include "persistence/Persistence.h"
#include <string>
#include <memory>
#include <folly/Optional.h>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>
#include <folly/Conv.h>


using namespace std;
using namespace folly;

namespace relevanced {
namespace persistence {

CentroidMetadataDb::CentroidMetadataDb(shared_ptr<PersistenceIf> persistence)
  : persistence_(persistence) {}

Future<Optional<uint64_t>> CentroidMetadataDb::getCreatedTimestamp(const std::string& centroidId) {
  Optional<uint64_t> result;
  return persistence_->getCentroidMetadata(centroidId, "created").then([](Optional<string> keyVal) {
    Optional<uint64_t> result;
    if (keyVal.hasValue()) {
      result.assign(folly::to<uint64_t>(keyVal.value()));
    }
    return result;
  });
}

Future<Optional<uint64_t>> CentroidMetadataDb::getLastCalculatedTimestamp(const std::string& centroidId) {
  Optional<uint64_t> result;
  return persistence_->getCentroidMetadata(centroidId, "lastCalculated").then([](Optional<string> keyVal) {
    Optional<uint64_t> result;
    if (keyVal.hasValue()) {
      result.assign(folly::to<uint64_t>(keyVal.value()));
    }
    return result;
  });
}

Future<Optional<uint64_t>> CentroidMetadataDb::getLastDocumentChangeTimestamp(const std::string& centroidId) {
  Optional<uint64_t> result;
  return persistence_->getCentroidMetadata(centroidId, "lastDocumentChange").then([](Optional<string> keyVal) {
    Optional<uint64_t> result;
    if (keyVal.hasValue()) {
      result.assign(folly::to<uint64_t>(keyVal.value()));
    }
    return result;
  });
}

Future<Try<bool>> CentroidMetadataDb::setCreatedTimestamp(const std::string& centroidId, uint64_t timestamp) {
  auto value = folly::to<string>(timestamp);
  return persistence_->setCentroidMetadata(centroidId, "created", value);
}

Future<Try<bool>> CentroidMetadataDb::setLastCalculatedTimestamp(const std::string& centroidId, uint64_t timestamp) {
  auto value = folly::to<string>(timestamp);
  return persistence_->setCentroidMetadata(centroidId, "lastCalculated", value);
}

Future<Try<bool>> CentroidMetadataDb::setLastDocumentChangeTimestamp(const std::string& centroidId, uint64_t timestamp) {
  auto value = folly::to<string>(timestamp);
  return persistence_->setCentroidMetadata(centroidId, "lastDocumentChange", value);
}

}
}