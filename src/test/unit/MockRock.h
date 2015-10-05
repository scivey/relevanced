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
  MOCK_METHOD2(put, bool(string, rocksdb::Slice));
  MOCK_METHOD1(get, string(const string&));
  MOCK_METHOD2(get, bool(const string&, string&));
  MOCK_METHOD1(exists, bool(const string&));
  MOCK_METHOD1(del, bool(const string&));
  bool iterRange(const std::string &start, const std::string &end, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override {
    return false;
  }
  bool iterRangeFromKey(const std::string &start, size_t, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override {
    return false;
  }
  bool iterRangeFromKeyOffset(const std::string &start, size_t, size_t, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override {
    return false;
  }
  bool iterPrefix(const std::string &prefix, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override {
    return false;
  }
  bool iterPrefixFromOffset(const std::string &prefix, size_t, size_t, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override {
    return false;
  }
  bool iterAll(std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override {
    return false;
  }
};
