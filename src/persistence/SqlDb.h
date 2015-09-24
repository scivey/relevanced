#pragma once
#include <cassert>
#include <string>
#include <cstring>
#include <tuple>
#include <atomic>

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <folly/Format.h>
#include <glog/logging.h>

#include "sqlUtil.h"

namespace persistence {

class SqlDb {
protected:
  const std::string fileName_;
  sqlite3 *db_ {nullptr};
  std::atomic<bool> inTransaction_ {false};
public:
  SqlDb(std::string fileName);
  bool tableExists(const char *name);
  bool tableExists(const std::string &name);
  ~SqlDb();

  template<typename ...Types>
  std::vector<std::tuple<Types...>> exec(const char *sql) {
    const char *unusedPtr;
    sqlite3_stmt *prepared;
    int rc = sqlite3_prepare_v2(db_, sql, strlen(sql), &prepared, &unusedPtr);
    if (rc != SQLITE_OK) {
      LOG(INFO) << "bad rc on sql: " << sql;
    }
    assert(rc == SQLITE_OK);
    auto result = sqlUtil::getSqlRows<Types...>(prepared);
    sqlite3_finalize(prepared);
    return result;
  }

  template<typename ...Types>
  std::vector<std::tuple<Types...>> exec(const std::string &sql) {
    return exec<Types...>(sql.c_str());
  }

  bool exec(const char *sql) {
    const char *unusedPtr;
    sqlite3_stmt *prepared;
    int rc = sqlite3_prepare_v2(db_, sql, strlen(sql), &prepared, &unusedPtr);
    if (rc != SQLITE_OK) {
      LOG(INFO) << "bad rc on sql: " << sql;
    }
    assert(rc == SQLITE_OK);
    sqlite3_finalize(prepared);
    return true;
  }

  template<typename ...ResultTypes>
  std::vector<std::tuple<ResultTypes...>> execPrepared(sqlite3_stmt *prepared) {
    auto result = sqlUtil::getSqlRows<ResultTypes...>(prepared);
    return result;
  }

  bool prepare(const char *sql, sqlite3_stmt **target) {
    const char *unusedPtr;
    sqlite3_prepare_v2(db_, sql, strlen(sql), target, &unusedPtr);
    return true;
  }

  sqlite3_stmt* prepare(const char *sql) {
    sqlite3_stmt *result;
    const char *unusedPtr;
    int rc = sqlite3_prepare_v2(db_, sql, strlen(sql), &result, &unusedPtr);
    assert (rc == 0);
    return result;
  }

  bool beginTransaction();
  bool endTransaction();

};

} // persistence

