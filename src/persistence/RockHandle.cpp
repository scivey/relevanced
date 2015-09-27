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
  auto status = rocksdb::OptimisticTransactionDB::Open(options_, dbPath_.c_str(), &txnDbPtr);
  assert(status.ok());
  assert(txnDbPtr != nullptr);
  txnDb_.reset(txnDbPtr);
  db_ = txnDb_->GetBaseDB();
}

bool RockHandle::put(string key, string val) {
  LOG(INFO) << "persisting: " << key << " -> " << val.substr(0,20);
  auto status = db_->Put(writeOptions_, key, val);
  return status.ok();
}

bool RockHandle::put(string key, rocksdb::Slice val) {
  auto status = db_->Put(writeOptions_, key, val);
  return status.ok();
}

string RockHandle::get(const string &key) {
  string val;
  auto status = db_->Get(readOptions_, key, &val);
  return val;
}

bool RockHandle::get(const string &key, string &result) {
  auto status = db_->Get(readOptions_, key, &result);
  return status.ok();
}

bool RockHandle::exists(const string &key) {
  string val;
  auto status = db_->Get(readOptions_, key, &val);
  return !status.IsNotFound();
}

bool RockHandle::del(const string &key) {
  if (!exists(key)) {
    return false;
  }
  auto status = db_->Delete(writeOptions_, key);
  return true;
}

bool RockHandle::iterRange(const string &start, const string &end, function<void (rocksdb::Iterator*, function<void()>)> iterFn) {
  rocksdb::Iterator *it = db_->NewIterator(readOptions_);
  bool stop = false;
  bool foundAny = false;
  function<void ()> escapeFn([&stop](){
    stop = true;
  });
  for (it->Seek(start); it->Valid() && it->key().ToString() < end; it->Next()) {
    foundAny = true;
    iterFn(it, escapeFn);
    if (stop) {
      break;
    }
  }
  return foundAny;
}

bool RockHandle::iterPrefix(const string &prefix, function<void (rocksdb::Iterator*, function<void()>)> iterFn) {
    string start = prefix + ":";
    string end = prefix + ";";
    return iterRange(start, end, iterFn);
}

bool RockHandle::iterAll(function<void (rocksdb::Iterator*, function<void()>)> iterFn) {
  return iterRange("a", "zzzzz", iterFn);
}
} // persistence

