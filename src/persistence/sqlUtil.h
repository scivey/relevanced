#pragma once
#include <cassert>
#include <string>
#include <cstring>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <folly/Format.h>
#include <folly/futures/Unit.h>
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

template<typename T>
struct BindSqlParam {
  static int bind(sqlite3_stmt *stmt, int n, T *t) {
    LOG(INFO) << "default sqlite3_bind!";
    return sqlite3_bind_null(stmt, n);
  }
};

template<>
struct BindSqlParam<int> {
  static int bind(sqlite3_stmt *stmt, int n, int param) {
    return sqlite3_bind_int(stmt, n, param);
  }
};

template<>
struct BindSqlParam<double> {
  static int bind(sqlite3_stmt *stmt, int n, double param) {
    return sqlite3_bind_double(stmt, n, param);
  }
};

template<>
struct BindSqlParam<char*> {
  static int bind(sqlite3_stmt *stmt, int n, char* param) {
    return sqlite3_bind_text(stmt, n, param, -1, SQLITE_TRANSIENT);
  }
};

template<>
struct BindSqlParam<std::string*> {
  static int bind(sqlite3_stmt *stmt, int n, std::string *param) {
    char *data = (char*) param->c_str();
    return sqlite3_bind_text(stmt, n, data, -1, SQLITE_TRANSIENT);
  }
};

template<>
struct BindSqlParam<folly::Unit> {
  static int bind(sqlite3_stmt *stmt, int n, folly::Unit &unit) {
    return sqlite3_bind_null(stmt, n);
  }
};


template<typename T>
bool bindParam(sqlite3_stmt *stmt, int n, T param) {
  return BindSqlParam<T>::bind(stmt, n, param) == 0;
}

template<typename T>
bool bindParams(sqlite3_stmt *stmt, T param) {
  return bindParam<T>(stmt, 1, param);
}

template<typename T, typename T2>
bool bindParams(sqlite3_stmt *stmt, T param, T2 param2) {
  return bindParam<T>(stmt, 1, param) && bindParam<T2>(stmt, 2, param2);
}

template<typename T, typename T2, typename T3>
bool bindParams(sqlite3_stmt *stmt, T param, T2 param2, T3 param3) {
  return bindParam<T>(stmt, 1, param)
      && bindParam<T2>(stmt, 2, param2)
      && bindParam<T3>(stmt, 3, param3);
}

template<typename T, typename T2, typename T3, typename T4>
bool bindParams(sqlite3_stmt *stmt, T param, T2 param2, T3 param3, T4 param4) {
  return bindParam<T>(stmt, 1, param)
      && bindParam<T2>(stmt, 2, param2)
      && bindParam<T3>(stmt, 3, param3)
      && bindParam<T4>(stmt, 4, param4);
}

class Binding {
public:
  sqlite3_stmt *stmt;

  template<typename T>
  Binding(sqlite3_stmt *stmt, T t): stmt(stmt) {
    if (!bindParams(stmt, t)) {
      LOG(INFO) << "error binding: " << t;
      assert(false);
    }
  }

  template<typename T, typename T2>
  Binding(sqlite3_stmt *stmt, T t, T2 t2): stmt(stmt) {
    if (!bindParams(stmt, t, t2)) {
      LOG(INFO) << "error binding: " << t << "\t" << t2;
      assert(false);
    }
  }

  template<typename T, typename T2, typename T3>
  Binding(sqlite3_stmt *stmt, T t, T2 t2, T3 t3): stmt(stmt) {
    if (!bindParams(stmt, t, t2, t3)) {
      LOG(INFO) << "error binding: " << t << "\t" << t2 << "\t" << t3;
      assert(false);
    }
  }

  template<typename T, typename T2, typename T3, typename T4>
  Binding(sqlite3_stmt *stmt, T t, T2 t2, T3 t3, T4 t4): stmt(stmt) {
    if (!bindParams(stmt, t, t2, t3, t4)) {
      LOG(INFO) << "error binding: " << t << "\t" << t2 << "\t" << t3 << "\t" << t4;
      assert(false);
    }
  }

  ~Binding() {
    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
  }
};

} // sqlUtil
} // persistence

