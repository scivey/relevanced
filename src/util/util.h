#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <unordered_map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace relevanced {
namespace util {


// used to make phony shared_ptr<T> instances that don't call `free()`
template <typename T>
struct NoDelete {
  void operator()(T *) const {}
};

template <typename T>
void defaultDelete(T *t) {
  delete t;
}

// a type-erased version of std::unique_ptr
template <typename T>
struct UniquePointer {
  std::unique_ptr<T, std::function<void(T *) >> ptr;
  UniquePointer(T *t, std::function<void(T *) > deleteFunc) {
    std::unique_ptr<T, std::function<void(T *) >> temp(t, deleteFunc);
    ptr = std::move(temp);
  }
  UniquePointer(T *t) {
    std::unique_ptr<T, std::function<void(T *) >> temp(t, defaultDelete<T>);
    ptr = std::move(temp);
  }

  UniquePointer(const UniquePointer<T> &other) = delete;

  T* release() {
    return ptr.release();
  }

  UniquePointer(std::unique_ptr<T, std::function<void(T *) >> &&otherPtr)
      : ptr(std::move(otherPtr)) {}

  UniquePointer(UniquePointer<T> &&other) noexcept
      : UniquePointer(std::move(other.ptr)) {}

  UniquePointer<T> &operator=(UniquePointer<T> &&other) noexcept {
    ptr = std::move(other.ptr);
    return *this;
  }

  T *get() { return ptr.get(); }

  T *operator->() { return ptr.get(); }
};

template <typename T>
class Counter {
 public:
  std::map<T, size_t> counts;
  void incrBy(const T &key, size_t amount) {
    if (counts.find(key) != counts.end()) {
      counts[key] += amount;
    } else {
      counts[key] = 1;
    }
  }
  void incr(const T &key) { incrBy(key, 1); }
};


template <typename T1, typename T2>
std::vector<T1> getSortedKeys(const std::map<T1, T2> &aMap) {
  std::vector<T1> result;
  for (auto &elem : aMap) {
    result.push_back(elem.first);
  }
  std::sort(result.begin(), result.end());
  return result;
}

template <typename TKey, typename TVal>
std::set<TKey> getKeySet(const std::map<TKey, TVal> &aMap) {
  std::set<TKey> result;
  for (auto &elem : aMap) {
    result.insert(elem.first);
  }
  return result;
}

template <typename TKey, typename TVal>
std::set<TKey> getKeySet(const std::unordered_map<TKey, TVal> &aMap) {
  std::set<TKey> result;
  for (auto &elem : aMap) {
    result.insert(elem.first);
  }
  return result;
}

template <typename T>
std::vector<T> vecOfSet(const std::set<T> &t) {
  std::vector<T> output;
  for (auto &elem : t) {
    output.push_back(elem);
  }
  return output;
}

std::string getUuid();

bool isOnlyAscii(const std::string &text);

int64_t getChronoEpochTime();
std::string sha1(const std::string &input);

} // util
} // relevanced
