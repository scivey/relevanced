#pragma once
#include <chrono>
#include <set>
#include <string>
#include <atomic>
#include <memory>
#include <folly/Synchronized.h>
#include <folly/ProducerConsumerQueue.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <folly/futures/helpers.h>
#include <folly/io/async/EventBase.h>
#include <glog/logging.h>

template<typename T>
class Debouncer {
  folly::Synchronized<std::set<T>> inFlight_;
  function<void (T)> onValueCb_;
  std::chrono::milliseconds interval_;
  std::chrono::milliseconds requeueDelay_;
  std::chrono::milliseconds initialDelay_;
  std::atomic<bool> stopping_ {false};
public:
  Debouncer(std::chrono::milliseconds initialDelay, std::chrono::milliseconds interval, function<void (T)> onValue): initialDelay_(initialDelay), interval_(interval), onValueCb_(onValue) {
    std::chrono::milliseconds diff(10);
    requeueDelay_ = interval_ + diff;
  }
  void stop() {
    stopping_ = true;
  }
  void removeDebounced(T &t) {
    SYNCHRONIZED(inFlight_) {
      if (inFlight_.find(t) != inFlight_.end()) {
        inFlight_.erase(t);
      }
    }
  }
  bool writeIfNotInFlight(T &t) {
    if (stopping_) {
      return false;
    }
    bool shouldWrite = false;
    SYNCHRONIZED(inFlight_) {
      if (inFlight_.find(t) == inFlight_.end()) {
        shouldWrite = true;
        inFlight_.insert(t);
      }
    }
    if (shouldWrite) {
      folly::makeFuture(t).delayed(initialDelay_).then([this](T elem) {
        if (!stopping_) {
          onValueCb_(elem);
        }
      });
      folly::makeFuture(t).delayed(interval_).then([this](T elem) {
        if (!stopping_) {
          removeDebounced(elem);
        }
      });
    }
    bool wrote = shouldWrite;
    return wrote;
  }
  void write(T t) {
    bool wrote = writeIfNotInFlight(t);
    if (!wrote) {
      folly::makeFuture(t).delayed(requeueDelay_).then([this](T toWrite) {
        if (!stopping_) {
          writeIfNotInFlight(toWrite);
        }
      });
    }
  }
};
