#pragma once
#include "RockHandle.h"
#include <memory>
#include <string>
#include <functional>
#include <rocksdb/slice.h>
#include <folly/Format.h>
#include <folly/Synchronized.h>

namespace persistence {

class InMemoryRockHandle: public RockHandleIf {
protected:
  folly::Synchronized<std::map<std::string, std::string>> data_;
public:
  std::string dbPath;
  InMemoryRockHandle(std::string dbPath);
  bool put(std::string key, rocksdb::Slice) override;
  std::string get(const std::string &key) override;
  bool get(const std::string &key, std::string &result) override;
  bool exists(const std::string &key) override;
  bool del(const std::string &key) override;
  bool iterRange(const std::string &start, const std::string &end, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override;
  bool iterAll(std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override;
  bool iterPrefix(const std::string &prefix, std::function<void (const std::string&, std::function<void(std::string&)>, std::function<void()>)> iterFn) override;
};


} // persistence

