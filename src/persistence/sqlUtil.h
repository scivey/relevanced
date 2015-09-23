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

namespace persistence {
namespace sqlUtil {

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
struct GetSqlColumn<std::string> {
  static std::string get(sqlite3_stmt *stmt, int n) {
    const unsigned char *text = sqlite3_column_text(stmt, n);
    const char *txt = (const char*) text;
    std::string res = txt;
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
std::tuple<T> getSqlRow(sqlite3_stmt *stmt) {
  return std::make_tuple(getSqlColumn<T>(stmt, 0));
}

template<typename T, typename T2>
std::tuple<T, T2> getSqlRow(sqlite3_stmt *stmt) {
  return std::make_tuple(getSqlColumn<T>(stmt, 0), getSqlColumn<T2>(stmt, 1));
}

template<typename T, typename T2, typename T3>
std::tuple<T, T2, T3> getSqlRow(sqlite3_stmt *stmt) {
  return std::make_tuple(
    getSqlColumn<T>(stmt, 0),
    getSqlColumn<T2>(stmt, 1),
    getSqlColumn<T3>(stmt, 2)
  );
}

template<typename T, typename T2, typename T3, typename T4>
std::tuple<T, T2, T3, T4> getSqlRow(sqlite3_stmt *stmt) {
  return std::make_tuple(
    getSqlColumn<T>(stmt, 0),
    getSqlColumn<T2>(stmt, 1),
    getSqlColumn<T3>(stmt, 2),
    getSqlColumn<T4>(stmt, 3)
  );
}

template<typename ...Types>
std::vector<std::tuple<Types...>> getSqlRows(sqlite3_stmt *stmt) {
  std::vector<std::tuple<Types...>> output;
  int rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
    auto elem = getSqlRow<Types...>(stmt);
    output.push_back(elem);
    rc = sqlite3_step(stmt);
  }
  return output;
}

} // sqlUtil
} // persistence

