#include "InMemoryRockHandle.h"
#include <memory>
#include <string>
#include <functional>
#include <rocksdb/slice.h>
#include <folly/Format.h>
#include <folly/Synchronized.h>

using namespace folly;
using namespace std;

namespace persistence {

InMemoryRockHandle::InMemoryRockHandle(string dbPath): dbPath(dbPath){}

bool InMemoryRockHandle::put(string key, rocksdb::Slice val) {
  SYNCHRONIZED(data_) {
    data_.insert(make_pair(key, val.ToString()));
  }
  return true;
}

string InMemoryRockHandle::get(const string &key) {
  string result {""};
  SYNCHRONIZED(data_) {
    if (data_.find(key) != data_.end()) {
      result = data_[key];
    }
  }
  return result;
}

bool InMemoryRockHandle::get(const string &key, string &result) {
  bool gotten;
  SYNCHRONIZED(data_) {
    if (data_.find(key) == data_.end()) {
      gotten = false;
    } else {
      result = data_[key];
      gotten = true;
    }
  }
  return gotten;
}

bool InMemoryRockHandle::exists(const string &key) {
  bool result;
  SYNCHRONIZED(data_) {
    if (data_.find(key) == data_.end()) {
      result = false;
    } else {
      result = true;
    }
  }
  return result;
}

bool InMemoryRockHandle::del(const string &key) {
  bool result;
  SYNCHRONIZED(data_) {
    if (data_.find(key) == data_.end()) {
      result = false;
    } else {
      data_.erase(key);
      result = true;
    }
  }
  return result;
}

bool InMemoryRockHandle::iterRange(const string &start, const string &end, function<void (const string&, function<void(string&)>, function<void()>)> iterFn) {
  // this is very inefficient.
  // since this is just for testing, I don't really care.
  // if I start to, I'll keep a prioqueue of the sorted keys
  vector<pair<string, string>> sortedKeys;
  SYNCHRONIZED(data_) {
    for (auto &elem: data_) {
      sortedKeys.push_back(elem);
    }
  }
  std::sort(sortedKeys.begin(), sortedKeys.end(), [](const pair<string, string> &p1, const pair<string, string> &p2) {
    return p1.first < p2.first;
  });
  bool stop = false;
  function<void ()> escapeFunc([&stop](){
    stop = true;
  });
  bool anyVisited = false;
  for (auto &elem: sortedKeys) {
    if (stop) {
      break;
    }
    if (elem.first < start) {
      continue;
    }
    if (elem.first >= end) {
      break;
    }
    anyVisited = true;
    function<void (string&)> readFunc([&elem](string &result){
      result = elem.second;
    });
    iterFn(elem.first, readFunc, escapeFunc);
  }
  return anyVisited;
}

bool InMemoryRockHandle::iterAll(function<void (const string&, function<void(string&)>, function<void()>)> iterFn) {
  return iterRange("a", "zzz", iterFn);
}

bool InMemoryRockHandle::iterPrefix(const string &prefix, function<void (const string&, function<void(string&)>, function<void()>)> iterFn) {
  string start = prefix + ":";
  string end = prefix + ";";
  return iterRange(start, end, iterFn);
}


} // persistence

