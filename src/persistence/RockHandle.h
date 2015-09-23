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

namespace {
  using namespace std;
  using namespace folly;
  using namespace rocksdb;
}

namespace persistence {

class RockHandleIf {
public:
  virtual bool put(std::string key, std::string val) = 0;
  virtual std::string get(const std::string &key) = 0;
  virtual bool exists(const std::string &key) = 0;
  virtual bool del(const std::string &key) = 0;
};

class RockHandle: public RockHandleIf {
protected:
  rocksdb::Options options_;
  rocksdb::ReadOptions readOptions_;
  rocksdb::WriteOptions writeOptions_;
  const std::string dbPath_;
  std::unique_ptr<rocksdb::OptimisticTransactionDB> txnDb_;
  rocksdb::DB *db_;
  rocksdb::Status status_;
public:
  RockHandle(std::string dbPath);
  bool put(std::string key, std::string val) override;
  std::string get(const std::string &key) override;
  bool exists(const std::string &key) override;
  bool del(const std::string &key) override;
};

} // persistence

