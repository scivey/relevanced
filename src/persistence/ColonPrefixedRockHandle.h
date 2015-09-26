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

namespace persistence {

  class ColonPrefixedRockHandle {
    rocksdb::Options options_;
    rocksdb::ReadOptions readOptions_;
    rocksdb::WriteOptions writeOptions_;
    const std::string dbPath_;
    std::unique_ptr<rocksdb::OptimisticTransactionDB> txnDb_;
    rocksdb::DB *db_;
  public:
    ColonPrefixedRockHandle(std::string dbPath);
    bool put(std::string key, std::string val);
    bool put(std::string key, rocksdb::Slice val);
    std::string get(const std::string &key);
    bool get(const std::string &key, std::string &result);
    bool exists(const std::string &key);
    bool del(const std::string &key);
    bool iter(const std::string &start, const std::string &end, std::function<void (rocksdb::Iterator*, std::function<void()>)> iterFn);
    bool iterPrefix(const std::string &prefix, std::function<void (rocksdb::Iterator*, std::function<void()>)> iterFn);
  };

} // persistence
