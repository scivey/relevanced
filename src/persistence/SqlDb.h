#pragma once
#include <cassert>
#include <string>
#include <cstring>
#include <tuple>
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


};

} // persistence

