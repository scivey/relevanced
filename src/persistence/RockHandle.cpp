
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
#include <rocksdb/cache.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/utilities/optimistic_transaction.h>
#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <folly/Format.h>
#include <folly/ScopeGuard.h>

#include "persistence/RockHandle.h"

using namespace std;
using namespace folly;
using namespace rocksdb;

namespace relevanced {
namespace persistence {

class ColonPrefixTransform : public rocksdb::SliceTransform {
 public:
  const char *Name() const override { return "ColonPrefixTransform"; }
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

RockHandle::RockHandle(string dbPath) : dbPath_(dbPath) {
  options_.env->SetBackgroundThreads(4);
  options_.create_if_missing = true;
  options_.write_buffer_size = 1024 * 1024 * 32;
  options_.max_write_buffer_number = 5;
  options_.min_write_buffer_number_to_merge = 2;
  options_.max_bytes_for_level_base = 1024 * 1024 * 64;
  options_.max_bytes_for_level_multiplier = 8;
  options_.target_file_size_base = options_.max_bytes_for_level_base / 10;
  options_.num_levels = 5;
  struct BlockBasedTableOptions table_options;
  size_t cacheCapacity = 1024 * 1024 * 64;
  size_t cacheShardBits = 4;
  table_options.cache_index_and_filter_blocks = true;
  table_options.block_cache = rocksdb::NewLRUCache(cacheCapacity, cacheShardBits);
  table_options.block_size = 1024 * 8;
  options_.table_factory.reset(NewBlockBasedTableFactory(table_options));
  openDb();
}

void RockHandle::closeDb() {
  auto db = db_.release();
  if (db != nullptr) {
    delete db;
  }
}

void RockHandle::openDb() {
  CHECK(db_.get() == nullptr);
  rocksdb::DB *dbPtr = nullptr;
  auto status = rocksdb::DB::Open(
      options_, dbPath_.c_str(), &dbPtr);
  CHECK(status.ok());
  CHECK(dbPtr != nullptr);
  db_.reset(dbPtr);
}

bool RockHandle::put(string key, rocksdb::Slice val) {
  auto status = db_->Put(writeOptions_, key, val);
  return status.ok();
}

string RockHandle::get(const string &key) {
  string val;
  auto status = db_->Get(readOptions_, key, &val);
  CHECK(status.ok());
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

bool RockHandle::iterRange(
    const string &start,
    const string &end,
    function<void(const string &, function<void(string &) >, function<void()>) >
        iterFn) {
  rocksdb::Iterator *it = db_->NewIterator(readOptions_);
  ScopeGuard guard = makeGuard([it]() { delete it; });
  (void) guard;
  bool foundAny = false;
  bool stop = false;
  function<void()> escapeFunc([&stop]() { stop = true; });
  function<void(string &) > readValFunc(
      [&it](string &result) { result = it->value().ToString(); });
  for (it->Seek(start); it->Valid() && it->key().ToString() < end; it->Next()) {
    foundAny = true;
    iterFn(it->key().ToString(), readValFunc, escapeFunc);
    if (stop) {
      break;
    }
  }
  return foundAny;
}

bool RockHandle::iterPrefix(
    const string &prefix,
    function<void(const string &, function<void(string &) >, function<void()>) >
        iterFn) {
  string start = prefix + ":";
  string end = prefix + ";";
  return iterRange(start, end, iterFn);
}

bool RockHandle::iterPrefixFromOffset(
    const string &prefix,
    size_t offset,
    size_t limitCount,
    function<void(const string &, function<void(string &) >, function<void()>) >
        iterFn) {
  string start = prefix + ":";
  string end = prefix + ";";
  size_t offsetSeen = 0;
  size_t limitSeen = 0;
  bool anySeen = false;
  iterRange(start, end,
            [&anySeen, &offsetSeen, &limitSeen, offset, limitCount, &iterFn](
                const string &key, function<void(string &) > read,
                function<void()> escape) {
              offsetSeen++;
              if (offsetSeen <= offset) {
                return;
              }
              anySeen = true;
              limitSeen++;
              if (limitSeen > limitCount) {
                escape();
                return;
              }
              iterFn(key, read, escape);
            });
  return anySeen;
}

bool RockHandle::iterPrefixFromMember(
    const string &prefix,
    const string &member,
    size_t limitCount,
    function<void(const string &, function<void(string &) >, function<void()>) >
        iterFn) {
  string start = sformat("{}:{}", prefix, member);
  string end = prefix + ";";
  size_t limitSeen = 0;
  bool anySeen = false;
  iterRange(start, end, [&anySeen, &limitSeen, limitCount, &iterFn](
                            const string &key, function<void(string &) > read,
                            function<void()> escape) {
    anySeen = true;
    limitSeen++;
    if (limitSeen > limitCount) {
      escape();
      return;
    }
    iterFn(key, read, escape);
  });
  return anySeen;
}

bool RockHandle::iterAll(function<void(
    const string &, function<void(string &) >, function<void()>) > iterFn) {
  string start = "a";
  string end = "zzz";
  return iterRange(start, end, iterFn);
}

// this method is only meant for testing purposes.
// THERE IS NO ATTEMPT TO SYNCHRONIZE WITH OTHER THREADS.
bool RockHandle::eraseEverything(){
  closeDb();
  auto status = rocksdb::DestroyDB(dbPath_, options_);
  openDb();
  return status.ok();
}

string RockHandle::getStatsDump() {
  string stats;
  db_->GetProperty("rocksdb.stats", &stats);
  return stats;
}

} // persistence
} // relevanced
