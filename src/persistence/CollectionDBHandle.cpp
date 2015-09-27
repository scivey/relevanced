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

bool CollectionDBHandle::addDocToCollection(const string &collId, const string &docId, bool isPositive) {
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
bool CollectionDBHandle::doesCollectionHaveDoc(const string &collId, const string &docId) {
  auto key = sformat("{}:{}", collId, docId);
  return collectionDocsHandle_->exists(key);
}
bool CollectionDBHandle::addPositiveDocToCollection(const string &collId, const string &docId) {
  bool isPositive = true;
  return addDocToCollection(collId, docId, isPositive);
}
bool CollectionDBHandle::addNegativeDocToCollection(const string &collId, const string &docId) {
  bool isPositive = false;
  return addDocToCollection(collId, docId, isPositive);
}
bool CollectionDBHandle::removeDocFromCollection(const string &collId, const string &docId) {
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
  collectionDocsHandle_->iterPrefix(collId, [&collectionKeys](rocksdb::Iterator *it, function<void()> escape) {
    collectionKeys.push_back(it->key().ToString());
  });
  for (auto &key: collectionKeys) {
    collectionDocsHandle_->del(key);
  }
  return true;
}
vector<string> CollectionDBHandle::listCollections() {
  vector<string> collections;
  collectionListHandle_->iterAll([&collections](rocksdb::Iterator *it, function<void()> escape) {
    collections.push_back(it->key().ToString());
  });
  return collections;
}
int CollectionDBHandle::getCollectionDocCount(const string &collId) {
  size_t count = 0;
  collectionDocsHandle_->iterPrefix(collId, [&count](rocksdb::Iterator *it, function<void()> escape) {
    count++;
  });
  return count;
}
vector<string> CollectionDBHandle::listCollectionDocs(const string &collId) {
  vector<string> docIds;
  collectionDocsHandle_->iterPrefix(collId, [&docIds, collId](rocksdb::Iterator *it, function<void()> escape) {
    auto key = it->key().ToString();
    auto offset = key.find(':');
    assert(offset != string::npos);
    docIds.push_back(key.substr(offset + 1));
  });
  return docIds;
}

vector<string> CollectionDBHandle::listPositiveCollectionDocs(const string &collId) {
  vector<string> docIds;
  collectionDocsHandle_->iterPrefix(collId, [&docIds](rocksdb::Iterator *it, function<void()> escape) {
    auto val = it->value().ToString();
    if (val == "1") {
      auto key = it->key().ToString();
      auto offset = key.find(':');
      assert(offset != string::npos);
      docIds.push_back(key.substr(offset + 1));
    }
  });
  return docIds;
}

vector<string> CollectionDBHandle::listNegativeCollectionDocs(const string &collId) {
  vector<string> docIds;
  collectionDocsHandle_->iterPrefix(collId, [&docIds](rocksdb::Iterator *it, function<void()> escape) {
    auto val = it->value().ToString();
    if (val == "0") {
      auto key = it->key().ToString();
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