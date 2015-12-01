#pragma once

#include <memory>
#include <atomic>
#include <type_traits>
#include <glog/logging.h>
namespace relevanced {
namespace util {

/*
  This is a variant of shared_ptr that can be safely modified
  while being accessed by multiple threads.

  It can't actually be modified; the base MultiVersionPtr<T>
  refers to the same T* instance throughout its life.

  Client code doesn't deal directly with MultiVersionPtr<T>;
  it's given a MultiVersionPtr<T>::Proxy instance that points at
  a given MultiVersionPtr<T> "parent" instance.

  Assigning a new T* to the Proxy instance doesn't change what
  its parent MultiVersionPtr<T> is pointing at.  Instead, it
  decrements the usage count on the existing parent and then
  constructs a new MultiVersionPtr<T> wrapping the given T*,
  which it then marks as its new parent.  This new parent
  has a reference count of 1.

  Copying a given Proxy instance will give a new Proxy that points
  at the same parent pointer.  However, if the source of the copy
  is modified so that it points at a new T*, the copied proxy
  will continue to point at the old one.

  This is all a long-winded way of letting us update a shared pointer
  while making sure that any other thread that may have just grabbed
  the previous T* is able to continue using it.

  And we can do it all without any locks.
  (Parent reassignment and reference counts are both done via CAS.)
*/

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