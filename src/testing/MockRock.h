#pragma once
#include <functional>
#include <string>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include "persistence/RockHandle.h"
#include "gmock/gmock.h"

using namespace std;

class MockRock : public relevanced::persistence::RockHandleIf {
 public:
  MOCK_METHOD2(put, bool(string, rocksdb::Slice));
  MOCK_METHOD1(get, string(const string &));
  MOCK_METHOD2(get, bool(const string &, string &));
  MOCK_METHOD1(exists, bool(const string &));
  MOCK_METHOD1(del, bool(const string &));
  MOCK_METHOD0(eraseEverything, bool());
  bool iterRange(const std::string&,
                 const std::string&,
                 std::function<void(const std::string &,
                                    std::function<void(std::string &) >,
                                    std::function<void()>) >) {
    return false;
  }
  bool iterPrefix(const std::string&,
                  std::function<void(const std::string &,
                                     std::function<void(std::string &) >,
                                     std::function<void()>) >) {
    return false;
  }
  bool iterPrefixFromOffset(
      const std::string &,
      size_t,
      size_t,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) >) {
    return false;
  }
  bool iterPrefixFromMember(
      const std::string &,
      const std::string &,
      size_t,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > ) {
    return false;
  }
  bool iterAll(std::function<void(const std::string &,
                                  std::function<void(std::string &) >,
                                  std::function<void()>) >) {
    return false;
  }
};
