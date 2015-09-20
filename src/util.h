#pragma once
#include <map>
#include <string>
#include <sstream>


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace {
  using namespace std;
}

namespace util {


string getUuid() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  ostringstream oss;
  oss << uuid;
  return oss.str();
}

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
