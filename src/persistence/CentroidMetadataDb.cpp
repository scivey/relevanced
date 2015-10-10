#include "persistence/CentroidMetadataDb.h"
#include "persistence/Persistence.h"
#include <string>
#include <memory>
#include <vector>
#include <folly/Optional.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>
#include <folly/Conv.h>


using namespace std;
using namespace folly;

namespace relevanced {
namespace persistence {

CentroidMetadataDb::CentroidMetadataDb(shared_ptr<PersistenceIf> persistence)
  : persistence_(persistence) {}

template<typename T>
Future<Optional<T>> getMetadata(shared_ptr<PersistenceIf> persistence, const string &centroidId, const string &metadataName) {
  return persistence->getCentroidMetadata(centroidId, metadataName).then([](Optional<string> keyVal) {
    Optional<T> result;
    if (keyVal.hasValue()) {
      result.assign(folly::to<T>(keyVal.value()));
    }
    return result;
  });
}

template<typename T>
Future<Try<bool>> setMetadata(shared_ptr<PersistenceIf> persistence, const string &centroidId, const string &metadataName, T value) {
  auto strVal = folly::to<string>(value);
  return persistence->setCentroidMetadata(centroidId, metadataName, strVal);
}

Future<Optional<uint64_t>> CentroidMetadataDb::getCreatedTimestamp(const string &centroidId) {
  return getMetadata<uint64_t>(persistence_, centroidId, "created");
}

Future<Optional<uint64_t>> CentroidMetadataDb::getLastCalculatedTimestamp(const string& centroidId) {
  return getMetadata<uint64_t>(persistence_, centroidId, "lastCalculated");
}

Future<Optional<uint64_t>> CentroidMetadataDb::getLastDocumentChangeTimestamp(const string& centroidId) {
  return getMetadata<uint64_t>(persistence_, centroidId, "lastDocumentChange");
}

Future<Try<bool>> CentroidMetadataDb::isCentroidUpToDate(const string& centroidId) {
  vector<Future<Optional<uint64_t>>> timestamps;
  timestamps.push_back(getLastCalculatedTimestamp(centroidId));
  timestamps.push_back(getLastDocumentChangeTimestamp(centroidId));
  return collect(timestamps).then([] (vector<Optional<uint64_t>> stamps) {
    auto lastCalculated = stamps.at(0);
    auto lastChanged = stamps.at(1);
    if (!lastChanged.hasValue()) {
      return Try<bool>(true);
    }
    if (!lastCalculated.hasValue()) {
      return Try<bool>(false);
    }
    if (lastChanged.value() > lastCalculated.value()) {
      return Try<bool>(false);
    }
    return Try<bool>(true);
  });

}

Future<Try<bool>> CentroidMetadataDb::setCreatedTimestamp(const string& centroidId, uint64_t timestamp) {
  return setMetadata(persistence_, centroidId, "created", timestamp);
}

Future<Try<bool>> CentroidMetadataDb::setLastCalculatedTimestamp(const string& centroidId, uint64_t timestamp) {
  return setMetadata(persistence_, centroidId, "lastCalculated", timestamp);
}

Future<Try<bool>> CentroidMetadataDb::setLastDocumentChangeTimestamp(const string& centroidId, uint64_t timestamp) {
  return setMetadata(persistence_, centroidId, "lastDocumentChange", timestamp);
}

}
}