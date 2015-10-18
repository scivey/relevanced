#pragma once

#include <memory>
#include <atomic>
#include <type_traits>

namespace relevanced {
namespace util {

template<typename T>
class MultiVersionPtr {
protected:
  T *ptr_;
  std::atomic<size_t> usage_ {0};

  T *getPtr() {
    return ptr_;
  }

public:
  void incrCount() {
    usage_++;
  }
  size_t decrCount() {
    auto count = usage_.fetch_sub(1) - 1;
    if (count == 0) {
      delete ptr_;
      ptr_ = nullptr;
    }
    return count;
  }
  ~MultiVersionPtr(){
    if (ptr_ != nullptr) {
      delete ptr_;
    }
  }

  class Proxy {
    protected:
      MultiVersionPtr<T> *base {nullptr};
      friend class MultiVersionPtr<T>;
      void decrBase() {
        if (base != nullptr) {
          if (base->decrCount() == 0) {
            delete base;
          }
        }
      }
      void clear() {
        base = nullptr;
      }
      void setBase(MultiVersionPtr<T> *parent) {
        if (base != parent) {
          decrBase();
          base = parent;
          base->incrCount();
        }
      }
    public:
      void reset(T *newParent) {
        auto base = new MultiVersionPtr<T>(newParent, this);
        ((void) base);
      }
      T *get() {
        return base->getPtr();
      }
      Proxy(){}
      Proxy(MultiVersionPtr<T> *parent) {
        setBase(parent);
      }
      Proxy(const Proxy& other) {
        setBase(other.base);
      }
      Proxy(Proxy &&other) {
        base = other.base;
        other.clear();
      }
      ~Proxy(){
        decrBase();
      }
      Proxy& operator=(const Proxy &other) {
        setBase(other.base);
        return *this;
      }
      T *operator->() {
        return base->getPtr();
      }
  };

  friend class Proxy;

  MultiVersionPtr(T *ptr, Proxy *firstProxy): ptr_(ptr) {
    firstProxy->setBase(this);
  }

  static Proxy create(T *ptr) {
    Proxy proxy;
    proxy.reset(ptr);
    return std::move(proxy);
  }
};

} // util
} // relevanced