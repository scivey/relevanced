#pragma once
#include <chrono>
#include <set>
#include <string>
#include <memory>
#include <folly/Synchronized.h>
#include <folly/ProducerConsumerQueue.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/io/async/EventBase.h>
#include <glog/logging.h>

template<typename T>
class DebouncedQueue {
  std::shared_ptr<folly::ProducerConsumerQueue<T>> pipe_;
  folly::Synchronized<std::set<T>> inFlight_;
  folly::EventBase *base_;
  std::chrono::milliseconds interval_;
  std::chrono::milliseconds requeueDelay_;
public:
  DebouncedQueue(folly::EventBase *base, size_t n, std::chrono::milliseconds interval): base_(base), interval_(interval) {
    pipe_.reset(
      new folly::ProducerConsumerQueue<T>(n)
    );
    std::chrono::milliseconds diff(10);
    requeueDelay_ = interval_ + diff;
  }
  void removeDebounced(T &t) {
    SYNCHRONIZED(inFlight_) {
      if (inFlight_.find(t) != inFlight_.end()) {
        inFlight_.erase(t);
      }
    }
  }
  bool writeIfNotInFlight(T &t) {
    bool shouldWrite = false;
    SYNCHRONIZED(inFlight_) {
      if (inFlight_.find(t) == inFlight_.end()) {
        shouldWrite = true;
        inFlight_.insert(t);
      }
    }
    if (shouldWrite) {
      base_->runInEventBaseThread([this, t](){
        pipe_->write(t);
      });
      folly::makeFuture(t).delayed(interval_).then([this](T elem) {
        removeDebounced(elem);
      });
    }
    return shouldWrite;
  }
  void write(T t) {
    if (!writeIfNotInFlight(t)) {
      folly::makeFuture(t).delayed(requeueDelay_).then([this](T elem) {
        writeIfNotInFlight(elem);
      });
    }
  }
  bool read(T &t) {
    return pipe_->read(t);
  }
};
