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
#include <rocksdb/table.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/utilities/optimistic_transaction.h>
#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <folly/Format.h>
#include "persistence/RockHandle.h"

using namespace std;
using namespace folly;
using namespace rocksdb;

namespace relevanced {
namespace persistence {

class ColonPrefixTransform: public rocksdb::SliceTransform {
public:
  const char* Name() const override {
    return "ColonPrefixTransform";
  }
  rocksdb::Slice Transform(const Slice &src) const override {
    assert(InDomain(src));
    auto str = src.ToString();
    return rocksdb::Slice(src.data(), str.find(':') + 1);
  }
  bool InDomain(const Slice &src) const override {
    auto str = src.ToString();
    return str.find(':') != string::npos;
  }
  bool InRange(const Slice &dst) const override {
    auto str = dst.ToString();
    return str.find(':') == (str.size() - 1);
  }
  bool SameResultWhenAppended(const Slice &prefix) const override {
    if (prefix[prefix.size() - 1] == ':') {
      return true;
    }
    return false;
  }
};

RockHandle::RockHandle(string dbPath): dbPath_(dbPath) {
  options_.IncreaseParallelism();
  options_.OptimizeLevelStyleCompaction();
  options_.create_if_missing = true;
  options_.prefix_extractor.reset(new ColonPrefixTransform());
  options_.memtable_prefix_bloom_bits = 100000000;
  options_.memtable_prefix_bloom_probes = 6;
  struct BlockBasedTableOptions table_options;
  table_options.filter_policy.reset(NewBloomFilterPolicy(10, true));
  options_.table_factory.reset(NewBlockBasedTableFactory(table_options));
  rocksdb::OptimisticTransactionDB *txnDbPtr = nullptr;
  auto status = rocksdb::OptimisticTransactionDB::Open(options_, dbPath_.c_str(), &txnDbPtr);
  assert(status.ok());
  assert(txnDbPtr != nullptr);
  txnDb_.reset(txnDbPtr);
  db_ = txnDb_->GetBaseDB();
}

bool RockHandle::put(string key, rocksdb::Slice val) {
  auto status = db_->Put(writeOptions_, key, val);
  return status.ok();
}

string RockHandle::get(const string &key) {
  string val;
  auto status = db_->Get(readOptions_, key, &val);
  assert(status.ok());
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

bool RockHandle::iterRange(const string &start, const string &end, function<void (const string&, function<void(string&)>, function<void()>)> iterFn) {
    rocksdb::Iterator *it = db_->NewIterator(readOptions_);
    bool foundAny = false;
    bool stop = false;
    function<void ()> escapeFunc([&stop](){
      stop = true;
    });
    function<void (string &)> readValFunc([&it](string &result){
      result = it->value().ToString();
    });
    for (it->Seek(start); it->Valid() && it->key().ToString() < end; it->Next()) {
      foundAny = true;
      iterFn(it->key().ToString(), readValFunc, escapeFunc);
      if (stop) {
        break;
      }
    }
    assert(it->status().ok());
    delete it;
    return foundAny;
}

bool RockHandle::iterPrefix(const string &prefix, function<void (const string&, function<void(string&)>, function<void()>)> iterFn) {
    string start = prefix + ":";
    string end = prefix + ";";
    return iterRange(start, end, iterFn);
}

bool RockHandle::iterAll(function<void (const string&, function<void(string&)>, function<void()>)> iterFn) {
    string start = "a";
    string end = "zzz";
    return iterRange(start, end, iterFn);
}

} // persistence
} // relevanced

