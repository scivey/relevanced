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

namespace relevanced {
namespace persistence {

class RockHandleIf {
 public:
  virtual bool put(std::string key, rocksdb::Slice) = 0;
  virtual std::string get(const std::string &key) = 0;
  virtual bool get(const std::string &key, std::string &result) = 0;
  virtual bool exists(const std::string &key) = 0;
  virtual bool del(const std::string &key) = 0;
  virtual bool iterRange(
      const std::string &start,
      const std::string &end,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > iterFn) = 0;
  virtual bool iterPrefix(
      const std::string &start,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > iterFn) = 0;
  virtual bool iterPrefixFromOffset(
      const std::string &start,
      size_t offset,
      size_t limitCount,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > iterFn) = 0;
  virtual bool iterPrefixFromMember(
      const std::string &prefix,
      const std::string &member,
      size_t limitCount,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > iterFn) = 0;
  virtual bool iterAll(std::function<void(const std::string &,
                                          std::function<void(std::string &) >,
                                          std::function<void()>) > iterFn) = 0;
  virtual bool eraseEverything() = 0;

  virtual ~RockHandleIf() = default;
};

class RockHandle : public RockHandleIf {
 protected:
  rocksdb::Options options_;
  rocksdb::ReadOptions readOptions_;
  rocksdb::WriteOptions writeOptions_;
  const std::string dbPath_;
  std::unique_ptr<rocksdb::OptimisticTransactionDB> txnDb_;
  rocksdb::DB *db_;
  void openDb();
  void closeDb();
 public:
  RockHandle(std::string dbPath);
  bool put(std::string key, rocksdb::Slice) override;
  std::string get(const std::string &key) override;
  bool get(const std::string &key, std::string &result) override;
  bool exists(const std::string &key) override;
  bool del(const std::string &key) override;
  bool iterRange(const std::string &start,
                 const std::string &end,
                 std::function<void(const std::string &,
                                    std::function<void(std::string &) >,
                                    std::function<void()>) > iterFn) override;
  bool iterPrefix(const std::string &start,
                  std::function<void(const std::string &,
                                     std::function<void(std::string &) >,
                                     std::function<void()>) > iterFn) override;
  bool iterPrefixFromOffset(
      const std::string &start,
      size_t offset,
      size_t limitCount,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > iterFn) override;
  bool iterPrefixFromMember(
      const std::string &prefix,
      const std::string &member,
      size_t limitCount,
      std::function<void(const std::string &,
                         std::function<void(std::string &) >,
                         std::function<void()>) > iterFn) override;
  bool iterAll(std::function<void(const std::string &,
                                  std::function<void(std::string &) >,
                                  std::function<void()>) > iterFn) override;

  bool eraseEverything() override;
};


} // persistence
} // relevanced
