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

bool InMemoryRockHandle::put(string key, string val) {
  SYNCHRONIZED(data_) {
    data_.insert(make_pair(key, val));
  }
  return true;
}

bool InMemoryRockHandle::put(string key, rocksdb::Slice val) {
  return put(key, val.ToString());
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

bool InMemoryRockHandle::iterRange(const string &start, const string &end, function<void (rocksdb::Iterator *it, function<void()>)> iterFn) {
  return false;
}

bool InMemoryRockHandle::iterAll(function<void (rocksdb::Iterator *it, function<void()>)> iterFn) {
  return false;
}


} // persistence

