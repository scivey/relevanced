#pragma once
#include <map>
namespace {
  using namespace std;
}

namespace util {

template<typename T>
class Counter {
public:
  map<T, size_t> counts;
  void incrBy(const T &key, size_t amount) {
    if (counts.find(key) != counts.end()) {
      counts[key] += amount;
    }
    counts[key] = 1;
  }
  void incr(const T &key) {
    incrBy(key, 1);
  }
};

} // util
