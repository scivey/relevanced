#include <cassert>
#include <string>
#include <cstring>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <folly/Format.h>
#include <glog/logging.h>

#include "SqlDb.h"
#include "sqlUtil.h"

using namespace std;
using namespace folly;

namespace persistence {

SqlDb::SqlDb(string fileName): fileName_(fileName){
  int rc = sqlite3_open(fileName_.c_str(), &db_);
  assert(rc == 0);
}

bool SqlDb::tableExists(const char *name) {
  auto query = folly::sformat(
    "select count(*) from sqlite_master where name='{}';", name
  );
  auto rows = exec<int>(query.c_str());
  assert(1 == rows.size());
  return std::get<0>(rows.at(0)) == 1;
}

bool SqlDb::tableExists(const string &name) {
  return tableExists(name.c_str());
}

bool SqlDb::beginTransaction() {
  if (!inTransaction_) {
    exec("BEGIN TRANSACTION;");
    inTransaction_ = true;
    return true;
  }
  return false;
}

bool SqlDb::endTransaction() {
  if (!inTransaction_) {
    return false;
  }
  exec("END TRANSACTION;");
  inTransaction_ = false;
  return true;
}

SqlDb::~SqlDb() {
  if (db_ != nullptr) {
    sqlite3_close(db_);
  }
}

} // persistence

