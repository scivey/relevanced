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
    DCHECK(ptr_ != nullptr);
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
      std::atomic<MultiVersionPtr<T>*> base {nullptr};
      friend class MultiVersionPtr<T>;
      void decrBase() {
        auto currentBase = base.exchange(nullptr);
        if (currentBase != nullptr) {
          if (currentBase->decrCount() == 0) {
            delete currentBase;
          }
        }
      }
      MultiVersionPtr<T>* clearAtomic(){
        auto prevBase = base.exchange(nullptr);
        return prevBase;
      }
      void setBase(MultiVersionPtr<T> *parent) {
        for (;;) {
          auto prevBase = base.load();
          if (prevBase == parent) {
            break;
          }
          if (base.compare_exchange_strong(prevBase, parent)) {
            if (parent != nullptr) {
              parent->incrCount();
            }
            if (prevBase != nullptr) {
              if (prevBase->decrCount() == 0) {
                delete prevBase;
              }
            }
            break;
          }
        }
      }
    public:
      void reset(T *newParent) {
        DCHECK(newParent != nullptr);
        auto base = new MultiVersionPtr<T>(newParent, this);
        ((void) base);
      }
      T *get() {
        auto currentBase = base.load();
        DCHECK(currentBase != nullptr);
        auto result = currentBase->getPtr();
        DCHECK(result != nullptr);
        return result;
      }
      Proxy(){}
      Proxy(MultiVersionPtr<T> *parent) {
        DCHECK(parent != nullptr);
        setBase(parent);
      }
      Proxy(const Proxy& other) {
        setBase(other.base.load());
      }
      Proxy(Proxy &&other) {
        setBase(other.base.load());
      }
      ~Proxy(){
        decrBase();
      }
      Proxy& operator=(const Proxy &other) {
        setBase(other.base.load());
        return *this;
      }
      T *operator->() {
        auto currentBase = base.load();
        DCHECK(currentBase != nullptr);
        return currentBase->getPtr();
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