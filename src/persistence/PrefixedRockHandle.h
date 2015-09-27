#pragma once
#include <cassert>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <glog/logging.h>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <rocksdb/utilities/optimistic_transaction.h>
#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <folly/Format.h>

#include "RockHandle.h"

namespace persistence {

class PrefixedRockHandle: public RockHandleIf {
  rocksdb::Options options_;
  rocksdb::ReadOptions readOptions_;
  rocksdb::WriteOptions writeOptions_;
  const std::string dbPath_;
  std::unique_ptr<rocksdb::OptimisticTransactionDB> txnDb_;
  rocksdb::DB *db_;
public:
  PrefixedRockHandle(std::string dbPath);
  bool put(std::string key, std::string val) override;
  bool put(std::string key, rocksdb::Slice val) override;
  std::string get(const std::string &key) override;
  bool get(const std::string &key, std::string &result) override;
  bool exists(const std::string &key) override;
  bool del(const std::string &key) override;
  bool iterRange(const std::string &start, const std::string &end, std::function<void (rocksdb::Iterator*, std::function<void()>)> iterFn) override;
  bool iterPrefix(const std::string &prefix, std::function<void (rocksdb::Iterator*, std::function<void()>)> iterFn) override;
  bool iterAll(std::function<void (rocksdb::Iterator *it, std::function<void()>)> iterFn) override;
};

} // persistence
