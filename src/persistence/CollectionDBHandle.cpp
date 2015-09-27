#include "CollectionDBHandle.h"
#include "util.h"
#include "PrefixedRockHandle.h"
#include "RockHandle.h"
#include <string>
#include <vector>
#include <cassert>
#include <folly/Format.h>
#include <glog/logging.h>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/table.h>
using namespace std;
using namespace folly;
using util::UniquePointer;

namespace persistence {

CollectionDBHandle::CollectionDBHandle(UniquePointer<RockHandleIf> collectionDocsHandle, UniquePointer<RockHandleIf> collectionListHandle):
  collectionDocsHandle_(std::move(collectionDocsHandle)), collectionListHandle_(std::move(collectionListHandle)) {}

bool CollectionDBHandle::addDocumentToCollection(const string &collId, const string &docId, bool isPositive) {
  if (!doesCollectionExist(collId)) {
    return false;
  }
  string key = sformat("{}:{}", collId, docId);
  int iVal = 0;
  if (isPositive) {
    iVal = 1;
  }
  string value = sformat("{}", iVal);
  if (collectionDocsHandle_->exists(key)) {
    return false;
  }
  assert(collectionDocsHandle_->put(key, value));
  return true;
}

bool CollectionDBHandle::createCollection(const string &collId) {
  if (collectionListHandle_->exists(collId)) {
    return false;
  }
  string data = "1";
  collectionListHandle_->put(collId, data);
  return true;
}

bool CollectionDBHandle::doesCollectionExist(const string &collId) {
  return collectionListHandle_->exists(collId);
}
bool CollectionDBHandle::doesCollectionHaveDocument(const string &collId, const string &docId) {
  auto key = sformat("{}:{}", collId, docId);
  return collectionDocsHandle_->exists(key);
}
bool CollectionDBHandle::addPositiveDocumentToCollection(const string &collId, const string &docId) {
  bool isPositive = true;
  return addDocumentToCollection(collId, docId, isPositive);
}
bool CollectionDBHandle::addNegativeDocumentToCollection(const string &collId, const string &docId) {
  bool isPositive = false;
  return addDocumentToCollection(collId, docId, isPositive);
}
bool CollectionDBHandle::removeDocumentFromCollection(const string &collId, const string &docId) {
  auto key = sformat("{}:{}", collId, docId);
  if (!collectionDocsHandle_->exists(key)) {
    return false;
  }
  collectionDocsHandle_->del(key);
  return true;
}
bool CollectionDBHandle::deleteCollection(const string &collId) {
  if (!collectionListHandle_->exists(collId)) {
    return false;
  }
  vector<string> collectionKeys;
  collectionDocsHandle_->iterPrefix(collId, [&collectionKeys](const string &key, function<void (string&)> read, function<void()> escape) {
    collectionKeys.push_back(key);
  });
  for (auto &key: collectionKeys) {
    collectionDocsHandle_->del(key);
  }
  return true;
}
vector<string> CollectionDBHandle::listCollections() {
  vector<string> collectionIds;
  collectionListHandle_->iterAll([&collectionIds](const string &key, function<void (string&)> read, function<void()> escape) {
    collectionIds.push_back(key);
  });
  return collectionIds;
}
int CollectionDBHandle::getCollectionDocumentCount(const string &collId) {
  size_t count = 0;
  collectionDocsHandle_->iterPrefix(collId, [&count](const string &key, function<void (string&)> read, function<void()> escape) {
    count++;
  });
  return count;
}
vector<string> CollectionDBHandle::listCollectionDocuments(const string &collId) {
  vector<string> docIds;
  collectionDocsHandle_->iterPrefix(collId, [&docIds](const string &key, function<void (string&)> read, function<void()> escape) {
    auto offset = key.find(':');
    assert(offset != string::npos);
    docIds.push_back(key.substr(offset + 1));
  });
  return docIds;
}

vector<string> CollectionDBHandle::listPositiveCollectionDocuments(const string &collId) {
  vector<string> docIds;
  collectionDocsHandle_->iterPrefix(collId, [&docIds](const string &key, function<void (string&)> read, function<void()> escape) {
    string val;
    read(val);
    if (val == "1") {
      auto offset = key.find(':');
      assert(offset != string::npos);
      docIds.push_back(key.substr(offset + 1));
    }
  });
  return docIds;
}
vector<string> CollectionDBHandle::listNegativeCollectionDocuments(const string &collId) {
  vector<string> docIds;
  collectionDocsHandle_->iterPrefix(collId, [&docIds](const string &key, function<void (string&)> read, function<void()> escape) {
    string val;
    read(val);
    if (val == "0") {
      auto offset = key.find(':');
      assert(offset != string::npos);
      docIds.push_back(key.substr(offset + 1));
    }
  });
  return docIds;
}

vector<string> CollectionDBHandle::listKnownDocuments() {
  vector<string> result;
  return result;
}

} // persistence