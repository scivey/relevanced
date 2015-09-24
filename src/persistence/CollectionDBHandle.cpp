#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <sqlite3.h>
#include <folly/Format.h>
#include <glog/logging.h>

#include "CollectionDBHandle.h"
#include "SqlDb.h"
#include "sqlUtil.h"

#include "util.h"
using namespace std;
using namespace folly;
using util::UniquePointer;

namespace persistence {

CollectionDBHandle::CollectionDBHandle(UniquePointer<SqlDb> sqlDb)
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
  if (collectionCountByNameStmt_ == nullptr) {
    collectionCountByNameStmt_ = sqlDb_->prepare(
      "select count(*) from collections where name=@N;"
    );
  }
  auto collId = (char*) collectionId.c_str();
  sqlUtil::Binding binding(collectionCountByNameStmt_, collId);
  auto res = sqlDb_->execPrepared<int>(binding.stmt);
  assert(1 == res.size());
  return std::get<0>(res.at(0)) != 0;
}

bool CollectionDBHandle::createCollection(const string &collectionId) {
  if (doesCollectionExist(collectionId)) {
    return false;
  }
  if (createCollectionStmt_ == nullptr) {
    createCollectionStmt_ = sqlDb_->prepare(
      "insert into collections (name) values (@N);"
    );
  }
  auto collId = (char*) collectionId.c_str();
  sqlUtil::Binding binding(createCollectionStmt_, collId);
  sqlDb_->execPrepared<int>(binding.stmt);
  return true;
}

bool CollectionDBHandle::doesCollectionHaveDoc(const string &collectionId, const string &docId) {
  if (doesCollectionHaveDocStmt_ == nullptr) {
    doesCollectionHaveDocStmt_ = sqlDb_->prepare(
      "select count(*) from collection_docs where collection_name=@N and document_id=@D;"
    );
  }
  auto collId = (char*) collectionId.c_str();
  auto docI = (char*) docId.c_str();
  sqlUtil::Binding binding(doesCollectionHaveDocStmt_, collId, docI);
  auto res = sqlDb_->execPrepared<int>(binding.stmt);
  assert(1 == res.size());
  return std::get<0>(res.at(0)) != 0;
}

bool CollectionDBHandle::addDocToCollection(const string &collectionId, const string &docId, bool isPositive) {
  if (doesCollectionHaveDoc(collectionId, docId)) {
    return false;
  }
  if (addDocumentToCollectionStmt_ == nullptr) {
    addDocumentToCollectionStmt_ = sqlDb_->prepare(
      "insert into collection_docs(collection_name, document_id, is_positive) values(@C, @D, @P)"
    );
  }
  int posInt = 0;
  if (isPositive) {
    posInt = 1;
  }
  auto collId = (char*) collectionId.c_str();
  auto docI = (char*) docId.c_str();
  sqlUtil::Binding binding(addDocumentToCollectionStmt_, collId, docI, posInt);
  sqlDb_->execPrepared<int>(binding.stmt);
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
  if (listCollectionsStmt_ == nullptr) {
    listCollectionsStmt_ = sqlDb_->prepare("select name from collections;");
  }
  auto res = sqlDb_->execPrepared<string>(listCollectionsStmt_);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

int CollectionDBHandle::getCollectionDocCount(const string &collectionId) {
  if (!doesCollectionExist(collectionId)) {
    return 0;
  }
  if (getCollectionDocCountStmt_ == nullptr) {
    getCollectionDocCountStmt_ = sqlDb_->prepare(
      "select count(*) from collection_docs where collection_name=@N;"
    );
  }
  auto collId = (char*) collectionId.c_str();
  sqlUtil::Binding binding(getCollectionDocCountStmt_, collId);
  auto res = sqlDb_->execPrepared<int>(binding.stmt);
  assert(1 == res.size());
  return std::get<0>(res.at(0));
}

vector<string> CollectionDBHandle::listCollectionDocs(const string &collectionId) {
  vector<string> output;
  if (!doesCollectionExist(collectionId)) {
    return output;
  }
  if (listCollectionDocsStmt_ == nullptr) {
    listCollectionDocsStmt_ = sqlDb_->prepare(
      "select document_id from collection_docs where collection_name=@N;"
    );
  }
  auto collId = (char*) collectionId.c_str();
  sqlUtil::Binding binding(listCollectionDocsStmt_, collId);
  auto res = sqlDb_->execPrepared<string>(binding.stmt);
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
  if (listCollectionDocsOfTypeStmt_ == nullptr) {
    listCollectionDocsOfTypeStmt_ = sqlDb_->prepare(
      "select document_id from collection_docs where collection_name=@N and is_positive=@P order by document_id;"
    );
  }
  auto collId = (char*) collectionId.c_str();
  sqlUtil::Binding binding(listCollectionDocsOfTypeStmt_, collId, 1);
  auto res = sqlDb_->execPrepared<string>(binding.stmt);
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
  if (listCollectionDocsOfTypeStmt_ == nullptr) {
    listCollectionDocsOfTypeStmt_ = sqlDb_->prepare(
      "select document_id from collection_docs where collection_name=@N and is_positive=@P order by document_id;"
    );
  }
  auto collId = (char*) collectionId.c_str();
  sqlUtil::Binding binding(listCollectionDocsOfTypeStmt_, collId, 0);
  auto res = sqlDb_->execPrepared<string>(binding.stmt);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

vector<string> CollectionDBHandle::listKnownDocuments() {
  vector<string> output;
  if (listKnownDocumentsStmt_ == nullptr) {
    listKnownDocumentsStmt_ = sqlDb_->prepare(
      "select distinct document_id from collection_docs order by document_id;"
    );
  }
  auto res = sqlDb_->execPrepared<string>(listKnownDocumentsStmt_);
  for (auto &row: res) {
    output.push_back(std::get<0>(row));
  }
  return output;
}

} // persistence

