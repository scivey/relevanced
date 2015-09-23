#pragma once

#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <vector>
#include <eigen3/Eigen/Dense>

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

std::string getUuid();


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

double vectorMag(const Eigen::VectorXd &vec, size_t count);

} // util
