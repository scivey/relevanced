#pragma once

#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <functional>

#include <vector>
#include <memory>
#include <eigen3/Eigen/Dense>

namespace {
  using namespace std;
}

namespace util {

template<typename T>
void defaultDelete(T *t) {
    delete t;
}

// a type-erased version of unique_ptr
template<typename T>
struct UniquePointer {
    unique_ptr<T, function<void (T*)>> ptr;
    UniquePointer(T *t, function<void (T*)> deleteFunc) {
        unique_ptr<T, function<void (T*)>> temp(t, deleteFunc);
        ptr = std::move(temp);
    }
    UniquePointer(T *t) {
      unique_ptr<T, function<void (T*)>> temp(
          t, defaultDelete<T>
      );
      ptr = std::move(temp);
    }

    UniquePointer(const UniquePointer<T> &other) = delete;

    UniquePointer(unique_ptr<T, function<void (T*)>> &&otherPtr)
        : ptr(std::move(otherPtr)){}

    UniquePointer(UniquePointer<T>&& other) noexcept:
        UniquePointer(std::move(other.ptr)) {}

    T* operator->() {
        return ptr.get();
    }
};

template<typename T>
class Counter {
public:
  map<T, size_t> counts;
  void incrBy(const T &key, size_t amount) {
    if (counts.find(key) != counts.end()) {
      counts[key] += amount;
    } else {
      counts[key] = 1;
    }
  }
  void incr(const T &key) {
    incrBy(key, 1);
  }
};


template<typename T1, typename T2>
vector<T1> getSortedKeys(const map<T1, T2> &aMap) {
  vector<T1> result;
  for (auto &elem: aMap) {
    result.push_back(elem.first);
  }
  std::sort(result.begin(), result.end());
  return result;
}

template<typename T>
vector<T> vecOfSet(const set<T> &t) {
  vector<T> output;
  for (auto &elem: t) {
    output.push_back(elem);
  }
  return output;
}

std::string getUuid();
double vectorMag(const Eigen::VectorXd &vec, size_t count);

} // util
