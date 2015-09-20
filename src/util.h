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
    } else {
      counts[key] = 1;
    }
  }
  void incr(const T &key) {
    incrBy(key, 1);
  }
};

map<string, size_t> countWords(const vector<string> &words) {
  Counter<string> counter;
  for (auto &elem: words) {
    counter.incr(elem);
  }
  return std::move(counter.counts);
}

} // util
