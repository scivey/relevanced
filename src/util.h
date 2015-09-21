#pragma once
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <vector>

#include <cmath>

#include <eigen3/Eigen/Dense>
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

double vectorMag(const Eigen::VectorXd &vec, size_t count) {
  double accum = 0.0;
  for (size_t i = 0; i < count; i++) {
    accum += pow(vec(i), 2);
  }
  return sqrt(accum);
}

} // util
