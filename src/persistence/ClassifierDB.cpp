#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <glog/logging.h>
#include "ClassifierDB.h"
#include "ClassifierDBHandle.h"
#include "util.h"

using namespace std;
using namespace folly;
using namespace wangle;
using util::UniquePointer;
namespace persistence {

ClassifierDB::ClassifierDB(UniquePointer<ClassifierDBHandleIf> dbHandle, shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
  : dbHandle_(std::move(dbHandle)), threadPool_(threadPool) {
  }

void ClassifierDB::initialize() {
  LOG(INFO) << "ClassifierDB initialized.";
}

Future<bool> ClassifierDB::doesClassifierExist(const string &classifierId) {
  return threadPool_->addFuture([this, classifierId](){
    return dbHandle_->doesClassifierExist(classifierId);
  });
}

Future<bool> ClassifierDB::createClassifier(const string &classifierId) {
  threadPool_->addFuture([this, classifierId](){
    return dbHandle_->createClassifier(classifierId);
  });
  return makeFuture(true);
}

Future<bool> ClassifierDB::doesClassifierHaveDocument(const string &classifierId, const string &docId) {
  return threadPool_->addFuture([this, classifierId, docId](){
    return dbHandle_->doesClassifierHaveDocument(classifierId, docId);
  });
}

Future<bool> ClassifierDB::addPositiveDocumentToClassifier(const string &classifierId, const string &docId) {
  threadPool_->addFuture([this, classifierId, docId](){
    return dbHandle_->addPositiveDocumentToClassifier(classifierId, docId);
  });
  return makeFuture(true);
}

Future<bool> ClassifierDB::addNegativeDocumentToClassifier(const string &classifierId, const string &docId) {
  threadPool_->addFuture([this, classifierId, docId](){
    return dbHandle_->addNegativeDocumentToClassifier(classifierId, docId);
  });
  return makeFuture(true);
}

Future<bool> ClassifierDB::removeDocumentFromClassifier(const string &classifierId, const string &docId) {
  threadPool_->addFuture([this, classifierId, docId](){
    return dbHandle_->removeDocumentFromClassifier(classifierId, docId);
  });
  return makeFuture(true);
}

Future<bool> ClassifierDB::deleteClassifier(const string &classifierId) {
  threadPool_->addFuture([this, classifierId](){
    return dbHandle_->deleteClassifier(classifierId);
  });
  return makeFuture(true);
}

Future<vector<string>> ClassifierDB::listClassifiers() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listClassifiers();
  });
}

Future<int> ClassifierDB::getClassifierDocumentCount(const string &classifierId) {
  return threadPool_->addFuture([this, classifierId](){
    return dbHandle_->getClassifierDocumentCount(classifierId);
  });
}

Future<vector<string>> ClassifierDB::listAllClassifierDocuments(const string &classifierId) {
  return threadPool_->addFuture([this, classifierId](){
    return dbHandle_->listAllClassifierDocuments(classifierId);
  });
}

Future<vector<string>> ClassifierDB::listAllPositiveClassifierDocuments(const string &classifierId) {
  return threadPool_->addFuture([this, classifierId](){
    return dbHandle_->listAllPositiveClassifierDocuments(classifierId);
  });
}

Future<vector<string>> ClassifierDB::listAllNegativeClassifierDocuments(const string &classifierId) {
  return threadPool_->addFuture([this, classifierId](){
    return dbHandle_->listAllNegativeClassifierDocuments(classifierId);
  });
}

Future<vector<string>> ClassifierDB::listKnownDocuments() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listKnownDocuments();
  });
}

} // persistence
