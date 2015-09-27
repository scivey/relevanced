#pragma once
#include <functional>
#include <string>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include "persistence/RockHandle.h"
#include "gmock/gmock.h"

using namespace std;

class MockRock: public persistence::RockHandleIf {
public:
  MOCK_METHOD2(put, bool(string, string));
  MOCK_METHOD2(put, bool(string, rocksdb::Slice));
  MOCK_METHOD1(get, string(const string&));
  MOCK_METHOD2(get, bool(const string&, string&));
  MOCK_METHOD1(exists, bool(const string&));
  MOCK_METHOD1(del, bool(const string&));
  bool iterRange(const string &start, const string &end, function<void (rocksdb::Iterator*, function<void()>)> iterFn) override {
    return false;
  }
  bool iterAll(function<void (rocksdb::Iterator*, function<void()>)> iterFn) override {
    return false;
  }
  bool iterPrefix(const string &start, function<void (rocksdb::Iterator*, function<void()>)> iterFn) override {
    return false;
  }
};
