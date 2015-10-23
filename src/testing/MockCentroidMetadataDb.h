#pragma once

#include "gmock/gmock.h"
#include "persistence/CentroidMetadataDb.h"
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/Synchronized.h>
#include <folly/Format.h>

using namespace std;
using namespace folly;
using namespace relevanced;
using namespace relevanced::persistence;

class MockCentroidMetadataDb : public CentroidMetadataDbIf {
  Synchronized<map<string, uint64_t>> timestamps_;

  Future<Optional<uint64_t>> getOptionalKey(const string &key) {
    Optional<uint64_t> result;
    SYNCHRONIZED(timestamps_) {
      auto keyVal = timestamps_.find(key);
      if (keyVal != timestamps_.end()) {
        result.assign(keyVal->second);
      }
    }
    return result;
  }

  void setKey(const string &key, uint64_t value) {
    SYNCHRONIZED(timestamps_) { timestamps_[key] = value; }
  }

  Future<Try<bool>> setKeyFuture(const string &key, uint64_t value) {
    setKey(key, value);
    Try<bool> result(true);
    return makeFuture(result);
  }

 public:
  Future<Optional<uint64_t>> getCreatedTimestamp(const string &centroidId) {
    auto key = sformat("{}:created", centroidId);
    return getOptionalKey(key);
  }

  Future<Optional<uint64_t>> getLastCalculatedTimestamp(
      const string &centroidId) {
    auto key = sformat("{}:lastCalculated", centroidId);
    return getOptionalKey(key);
  }

  Future<Optional<uint64_t>> getLastDocumentChangeTimestamp(
      const string &centroidId) {
    auto key = sformat("{}:lastDocumentChange", centroidId);
    return getOptionalKey(key);
  }

  bool syncIsCentroidUpToDate(const string &centroidId) {
    auto lastCalculated = getLastCalculatedTimestamp(centroidId).get();
    auto lastDocumentChange = getLastDocumentChangeTimestamp(centroidId).get();
    if (!lastDocumentChange.hasValue()) {
      return true;
    }
    if (!lastCalculated.hasValue()) {
      return false;
    }
    if (lastDocumentChange.value() > lastCalculated.value()) {
      return false;
    }
    return true;
  }

  Future<Try<bool>> isCentroidUpToDate(const string &centroidId) {
    auto isUpToDate = syncIsCentroidUpToDate(centroidId);
    Try<bool> result(isUpToDate);
    return makeFuture(result);
  }

  Future<Try<bool>> setCreatedTimestamp(const string &centroidId,
                                        uint64_t val) {
    auto key = sformat("{}:created", centroidId);
    return setKeyFuture(key, val);
  }

  Future<Try<bool>> setLastCalculatedTimestamp(const string &centroidId,
                                               uint64_t val) {
    auto key = sformat("{}:lastCalculated", centroidId);
    return setKeyFuture(key, val);
  }

  Future<Try<bool>> setLastDocumentChangeTimestamp(const string &centroidId,
                                                   uint64_t val) {
    auto key = sformat("{}:lastDocumentChange", centroidId);
    return setKeyFuture(key, val);
  }
};
