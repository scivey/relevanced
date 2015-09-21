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

namespace {
  using namespace std;
  using namespace folly;
}

namespace persistence {


template<typename T>
struct GetSqlColumn {
  static T get(sqlite3_stmt *stmt, int n) {
    T t;
    LOG(INFO) << "default GetSqlColumn";
    return t;
  }
};

template<>
struct GetSqlColumn<int> {
  static int get(sqlite3_stmt *stmt, int n) {
    return sqlite3_column_int(stmt, n);
  }
};

template<>
struct GetSqlColumn<string> {
  static string get(sqlite3_stmt *stmt, int n) {
    const unsigned char *text = sqlite3_column_text(stmt, n);
    const char *txt = (const char*) text;
    string res = txt;
    return res;
  }
};

template<>
struct GetSqlColumn<double> {
  static double get(sqlite3_stmt *stmt, int n) {
    return sqlite3_column_double(stmt, n);
  }
};

template<typename T>
T getSqlColumn(sqlite3_stmt *stmt, int n) {
  return GetSqlColumn<T>::get(stmt, n);
}

template<typename T>
tuple<T> getSqlRow(sqlite3_stmt *stmt) {
  return make_tuple(getSqlColumn<T>(stmt, 0));
}

template<typename T, typename T2>
tuple<T, T2> getSqlRow(sqlite3_stmt *stmt) {
  return make_tuple(getSqlColumn<T>(stmt, 0), getSqlColumn<T2>(stmt, 1));
}

template<typename T, typename T2, typename T3>
tuple<T, T2, T3> getSqlRow(sqlite3_stmt *stmt) {
  return make_tuple(
    getSqlColumn<T>(stmt, 0),
    getSqlColumn<T2>(stmt, 1),
    getSqlColumn<T3>(stmt, 2)
  );
}

template<typename T, typename T2, typename T3, typename T4>
tuple<T, T2, T3, T4> getSqlRow(sqlite3_stmt *stmt) {
  return make_tuple(
    getSqlColumn<T>(stmt, 0),
    getSqlColumn<T2>(stmt, 1),
    getSqlColumn<T3>(stmt, 2),
    getSqlColumn<T4>(stmt, 3)
  );
}

template<typename ...Types>
vector<tuple<Types...>> getSqlRows(sqlite3_stmt *stmt) {
  vector<tuple<Types...>> output;
  int rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
    auto elem = getSqlRow<Types...>(stmt);
    output.push_back(elem);
    rc = sqlite3_step(stmt);
  }
  return output;
}

class SqlDb {
protected:
  const string fileName_;
  sqlite3 *db_ {nullptr};
public:
  SqlDb(string fileName): fileName_(fileName){
    int rc = sqlite3_open(fileName_.c_str(), &db_);
    assert(rc == 0);
  }
  template<typename ...Types>
  vector<tuple<Types...>> exec(const char *sql) {
    const char *unusedPtr;
    sqlite3_stmt *prepared;
    int rc = sqlite3_prepare_v2(db_, sql, strlen(sql), &prepared, &unusedPtr);
    if (rc != SQLITE_OK) {
      LOG(INFO) << "bad rc on sql: " << sql;
    }
    assert(rc == SQLITE_OK);
    auto result = getSqlRows<Types...>(prepared);
    sqlite3_finalize(prepared);
    return result;
  }
  template<typename ...Types>
  vector<tuple<Types...>> exec(const string &sql) {
    return exec<Types...>(sql.c_str());
  }
  bool tableExists(const char *name) {
    auto query = folly::sformat(
      "select count(*) from sqlite_master where name='{}';", name
    );
    auto rows = exec<int>(query.c_str());
    assert(1 == rows.size());
    return std::get<0>(rows.at(0)) == 1;
  }
  ~SqlDb() {
    if (db_ != nullptr) {
      sqlite3_close(db_);
    }
  }

};

} // persistence

