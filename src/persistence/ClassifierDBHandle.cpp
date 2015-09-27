#include "ClassifierDBHandle.h"
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

ClassifierDBHandle::ClassifierDBHandle(UniquePointer<RockHandleIf> classifierDocsHandle, UniquePointer<RockHandleIf> classifierListHandle):
  classifierDocsHandle_(std::move(classifierDocsHandle)), classifierListHandle_(std::move(classifierListHandle)) {}

bool ClassifierDBHandle::addDocumentToClassifier(const string &collId, const string &docId, bool isPositive) {
  if (!doesClassifierExist(collId)) {
    return false;
  }
  string key = sformat("{}:{}", collId, docId);
  int iVal = 0;
  if (isPositive) {
    iVal = 1;
  }
  string value = sformat("{}", iVal);
  if (classifierDocsHandle_->exists(key)) {
    return false;
  }
  assert(classifierDocsHandle_->put(key, value));
  return true;
}

bool ClassifierDBHandle::createClassifier(const string &collId) {
  if (classifierListHandle_->exists(collId)) {
    return false;
  }
  string data = "1";
  classifierListHandle_->put(collId, data);
  return true;
}

bool ClassifierDBHandle::doesClassifierExist(const string &collId) {
  return classifierListHandle_->exists(collId);
}
bool ClassifierDBHandle::doesClassifierHaveDocument(const string &collId, const string &docId) {
  auto key = sformat("{}:{}", collId, docId);
  return classifierDocsHandle_->exists(key);
}
bool ClassifierDBHandle::addPositiveDocumentToClassifier(const string &collId, const string &docId) {
  bool isPositive = true;
  return addDocumentToClassifier(collId, docId, isPositive);
}
bool ClassifierDBHandle::addNegativeDocumentToClassifier(const string &collId, const string &docId) {
  bool isPositive = false;
  return addDocumentToClassifier(collId, docId, isPositive);
}
bool ClassifierDBHandle::removeDocumentFromClassifier(const string &collId, const string &docId) {
  auto key = sformat("{}:{}", collId, docId);
  if (!classifierDocsHandle_->exists(key)) {
    return false;
  }
  classifierDocsHandle_->del(key);
  return true;
}
bool ClassifierDBHandle::deleteClassifier(const string &collId) {
  if (!classifierListHandle_->exists(collId)) {
    return false;
  }
  vector<string> classifierKeys;
  classifierDocsHandle_->iterPrefix(collId, [&classifierKeys](const string &key, function<void (string&)> read, function<void()> escape) {
    classifierKeys.push_back(key);
  });
  for (auto &key: classifierKeys) {
    classifierDocsHandle_->del(key);
  }
  return true;
}
vector<string> ClassifierDBHandle::listClassifiers() {
  vector<string> classifierIds;
  classifierListHandle_->iterAll([&classifierIds](const string &key, function<void (string&)> read, function<void()> escape) {
    classifierIds.push_back(key);
  });
  return classifierIds;
}
int ClassifierDBHandle::getClassifierDocumentCount(const string &collId) {
  size_t count = 0;
  classifierDocsHandle_->iterPrefix(collId, [&count](const string &key, function<void (string&)> read, function<void()> escape) {
    count++;
  });
  return count;
}
vector<string> ClassifierDBHandle::listAllClassifierDocuments(const string &collId) {
  vector<string> docIds;
  classifierDocsHandle_->iterPrefix(collId, [&docIds](const string &key, function<void (string&)> read, function<void()> escape) {
    auto offset = key.find(':');
    assert(offset != string::npos);
    docIds.push_back(key.substr(offset + 1));
  });
  return docIds;
}

vector<string> ClassifierDBHandle::listAllPositiveClassifierDocuments(const string &collId) {
  vector<string> docIds;
  classifierDocsHandle_->iterPrefix(collId, [&docIds](const string &key, function<void (string&)> read, function<void()> escape) {
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
vector<string> ClassifierDBHandle::listAllNegativeClassifierDocuments(const string &collId) {
  vector<string> docIds;
  classifierDocsHandle_->iterPrefix(collId, [&docIds](const string &key, function<void (string&)> read, function<void()> escape) {
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

vector<string> ClassifierDBHandle::listKnownDocuments() {
  vector<string> result;
  return result;
}

} // persistence