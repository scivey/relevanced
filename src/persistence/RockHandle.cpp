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

using namespace std;
using namespace folly;
using namespace rocksdb;

namespace persistence {

RockHandle::RockHandle(string dbPath): dbPath_(dbPath){
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

bool RockHandle::put(string key, string val) {
  status_ = db_->Put(writeOptions_, key, val);
  return status_.ok();
}

string RockHandle::get(const string &key) {
  string val;
  status_ = db_->Get(readOptions_, key, &val);
  return val;
}

bool RockHandle::exists(const string &key) {
  string val;
  status_ = db_->Get(readOptions_, key, &val);
  return !status_.IsNotFound();
}

bool RockHandle::del(const string &key) {
  if (!exists(key)) {
    return false;
  }
  status_ = db_->Delete(writeOptions_, key);
  return true;
}

} // persistence

