#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>

#include "CollectionDBHandle.h"
#include "SqlDb.h"
#include "util.h"
using namespace std;
using namespace folly;
using util::UniquePointer;

namespace persistence {

CollectionDBHandle::CollectionDBHandle(UniquePointer<SqlDbIf> sqlDb)
  : sqlDb_(std::move(sqlDb)) {}

bool CollectionDBHandle::ensureTables() {
  if (!sqlDb_->tableExists("collections")) {
    LOG(INFO) << "table 'collections' does not exist; creating...";
    string query =
      "create table collections (name text not null primary key);";
    auto res = sqlDb_->exec<int>(query);
  }
  if (!sqlDb_->tableExists("collection_docs")) {
    LOG(INFO) << "table 'collection_docs' does not exist; creating...";
    string query =
      "create table collection_docs "
      "(collection_name text not null, document_id text not null, is_positive int not null);";
    auto res = sqlDb_->exec<int>(query);
  }
  return true;
}

bool CollectionDBHandle::doesCollectionExist(const string &collectionId) {
  string query = sformat("select count(*) from collections where name='{}';", collectionId);
  auto res = sqlDb_->exec<int>(query);
  assert(1 == res.size());
  return std::get<0>(res.at(0)) != 0;
}

bool CollectionDBHandle::createCollection(const string &collectionId) {
  if (doesCollectionExist(collectionId)) {
    return false;
  }
  string query = sformat("insert into collections (name) values('{}');", collectionId);
  sqlDb_->exec<int>(query);
  return true;
}

bool CollectionDBHandle::doesCollectionHaveDoc(const string &collectionId, const string &docId) {
  string query = sformat("select count(*) from collection_docs where collection_name='{}' and document_id='{}';", collectionId, docId);
  auto res = sqlDb_->exec<int>(query);
  assert(1 == res.size());
  return std::get<0>(res.at(0)) != 0;
}

bool CollectionDBHandle::addDocToCollection(const string &collectionId, const string &docId, bool isPositive) {
  if (doesCollectionHaveDoc(collectionId, docId)) {
    return false;
  }
  int posInt = 0;
  if (isPositive) {
    posInt = 1;
  }
  string q1 = "insert into collection_docs(collection_name, document_id, is_positive) ";
  string q2 = sformat("values ('{}', '{}', {})", collectionId, docId, posInt);
  string query = q1 + q2;
  sqlDb_->exec<int>(query);
  return true;
}

bool CollectionDBHandle::addPositiveDocToCollection(const string &collectionId, const string &docId) {
  bool isPositive = true;
  return addDocToCollection(collectionId, docId, isPositive);
}

bool CollectionDBHandle::addNegativeDocToCollection(const string &collectionId, const string &docId) {
  bool isPositive = false;
  return addDocToCollection(collectionId, docId, isPositive);
}

bool CollectionDBHandle::removeDocFromCollection(const string &collectionId, const string &docId) {
  if (!doesCollectionHaveDoc(collectionId, docId)) {
    return false;
  }
  string query = folly::sformat("delete from collection_docs where collection_name='{}' and document_id='{}';", collectionId, docId);
  sqlDb_->exec<int>(query);
  return true;
}

bool CollectionDBHandle::deleteCollection(const string &collectionId) {
  if (!doesCollectionExist(collectionId)) {
    return false;
  }
  string docQuery = folly::sformat("delete from collection_docs where collection_name='{}';", collectionId);
  sqlDb_->exec<int>(docQuery);
  string collQuery = folly::sformat("delete from collections where name='{}';", collectionId);
  sqlDb_->exec<int>(collQuery);
  return true;
}

vector<string> CollectionDBHandle::listCollections() {
  vector<string> output;
  auto res = sqlDb_->exec<string>("select name from collections;");
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

int CollectionDBHandle::getCollectionDocCount(const string &collectionId) {
  if (!doesCollectionExist(collectionId)) {
    return 0;
  }
  string query = folly::sformat("select count(*) from collection_docs where collection_name='{}';", collectionId);
  auto res = sqlDb_->exec<int>(query);
  assert(1 == res.size());
  return std::get<0>(res.at(0));
}

vector<string> CollectionDBHandle::listCollectionDocs(const string &collectionId) {
  vector<string> output;
  if (!doesCollectionExist(collectionId)) {
    return output;
  }
  string docQuery = folly::sformat("select document_id from collection_docs where collection_name='{}' order by document_id;", collectionId);
  auto res = sqlDb_->exec<string>(docQuery);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

vector<string> CollectionDBHandle::listPositiveCollectionDocs(const string &collectionId) {
  vector<string> output;
  if (!doesCollectionExist(collectionId)) {
    return output;
  }
  string docQuery = folly::sformat("select document_id from collection_docs where collection_name='{}' and is_positive=1 order by document_id;", collectionId);
  auto res = sqlDb_->exec<string>(docQuery);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

vector<string> CollectionDBHandle::listNegativeCollectionDocs(const string &collectionId) {
  vector<string> output;
  if (!doesCollectionExist(collectionId)) {
    return output;
  }
  string docQuery = folly::sformat("select document_id from collection_docs where collection_name='{}' and is_positive=0 order by document_id;", collectionId);
  auto res = sqlDb_->exec<string>(docQuery);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

vector<string> CollectionDBHandle::listKnownDocuments() {
  vector<string> output;
  string query = "select distinct document_id from collection_docs order by document_id;";
  auto res = sqlDb_->exec<string>(query);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

} // persistence

