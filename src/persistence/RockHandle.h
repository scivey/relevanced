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

class RockHandle {
protected:
  rocksdb::Options options_;
  rocksdb::ReadOptions readOptions_;
  rocksdb::WriteOptions writeOptions_;
  const string dbPath_;
  unique_ptr<rocksdb::OptimisticTransactionDB> txnDb_;
  rocksdb::DB *db_;
  rocksdb::Status status_;
public:
  RockHandle(string dbPath): dbPath_(dbPath){
    options_.IncreaseParallelism();
    options_.OptimizeLevelStyleCompaction();
    options_.create_if_missing = true;
    rocksdb::OptimisticTransactionDB *txnDbPtr = nullptr;
    status_ = rocksdb::OptimisticTransactionDB::Open(options_, dbPath_.c_str(), &txnDbPtr);
    assert(status_.ok());
    assert(txnDbPtr != nullptr);
    txnDb_.reset(txnDbPtr);
    db_ = txnDb_->GetBaseDB();
  }
  bool put(string key, string val) {
    status_ = db_->Put(writeOptions_, key, val);
    return status_.ok();
  }
  string get(const string &key) {
    string val;
    status_ = db_->Get(readOptions_, key, &val);
    return val;
  }
  bool exists(const string &key) {
    string val;
    status_ = db_->Get(readOptions_, key, &val);
    return !status_.IsNotFound();
  }
  bool del(const string &key) {
    if (!exists(key)) {
      return false;
    }
    status_ = db_->Delete(writeOptions_, key);
    return true;
  }
  void runTxn(function<void (OptimisticTransaction*)> handler) {
    OptimisticTransactionOptions txnOptions_;
    OptimisticTransaction *txn = txnDb_->BeginTransaction(writeOptions_);
    handler(txn);
    delete txn;
  }
};

} // persistence

