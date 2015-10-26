#pragma once
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <thread>

#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Promise.h>
#include <folly/io/async/EventBase.h>
#include <folly/ProducerConsumerQueue.h>
#include <folly/Synchronized.h>
#include <glog/logging.h>

namespace relevanced {
namespace util {

template <typename T>
class Debouncer {
  folly::Synchronized<std::set<T>> inFlight_;
  std::atomic<std::chrono::milliseconds> initialDelay_;
  std::atomic<std::chrono::milliseconds> interval_;
  std::atomic<std::chrono::milliseconds> requeueDelay_;
  std::function<void(T)> onValueCb_;
  std::atomic<bool> stopping_{false};
  std::atomic<size_t> numInFlight_ {0};

 public:
  Debouncer(std::chrono::milliseconds initialDelay,
            std::chrono::milliseconds interval,
            std::function<void(T)> onValue)
      : initialDelay_(initialDelay), interval_(interval), onValueCb_(onValue) {
    std::chrono::milliseconds diff(10);
    requeueDelay_ = interval + diff;
  }
  void stop() { stopping_ = true; }
  void removeDebounced(T &t) {
    SYNCHRONIZED(inFlight_) {
      if (inFlight_.find(t) != inFlight_.end()) {
        inFlight_.erase(t);
        numInFlight_.fetch_sub(1);
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
        numInFlight_.fetch_add(1);
      }
    }
    if (shouldWrite) {
      folly::makeFuture(t)
          .delayed(initialDelay_)
          .then([this](T elem) {
            if (!stopping_) {
              onValueCb_(elem);
            }
          });
      folly::makeFuture(t).delayed(interval_).then([this](T elem) {
        removeDebounced(elem);
      });
    }
    bool wrote = shouldWrite;
    return wrote;
  }
  void write(T t) {
    bool wrote = writeIfNotInFlight(t);
    if (!wrote) {
      folly::makeFuture(t)
          .delayed(requeueDelay_)
          .then([this](T toWrite) {
            if (stopping_) {
              removeDebounced(toWrite);
            } else {
              writeIfNotInFlight(toWrite);
            }
          });
    }
  }
  void join() {
    for (;;) {
      auto inProgress = numInFlight_.load();
      if (inProgress == 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  void debug_setTimeouts(std::chrono::milliseconds initialDelay, std::chrono::milliseconds interval, std::chrono::milliseconds requeueDelay) {
    initialDelay_ = initialDelay;
    interval_ = interval;
    requeueDelay_ = requeueDelay;
  }
  void debug_setShortTimeouts() {
    debug_setTimeouts(
      std::chrono::milliseconds(10),
      std::chrono::milliseconds(10),
      std::chrono::milliseconds(1)
    );
  }
  void debug_setVeryShortTimeouts() {
    debug_setTimeouts(
      std::chrono::milliseconds(0),
      std::chrono::milliseconds(0),
      std::chrono::milliseconds(0)
    );
  }
};

} // util
} // relevanced
