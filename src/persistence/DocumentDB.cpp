#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "DocumentDB.h"
#include "DocumentDBHandle.h"
#include "RockHandle.h"
#include "ProcessedDocument.h"
#include "util"
using namespace std;
using namespace folly;
using namespace wangle;
using util::UniquePointer;
namespace persistence {

DocumentDB::DocumentDB(UniquePointer<DocumentDBHandleIf> dbHandle, shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
  : dbHandle_(std::move(dbHandle)), threadPool_(threadPool){}

Future<bool> DocumentDB::doesDocumentExist(const string &docId) {
  return threadPool_.addFuture([this, docId](){
    return dbHandle_->doesDocumentExist(docId);
  });
}

Future<bool> DocumentDB::deleteDocument(const string &docId) {
  return threadPool_.addFuture([this, docId](){
    return dbHandle_->deleteDocument(docId);
  });
}

Future<bool> DocumentDB::saveDocument(ProcessedDocument *doc) {
  return threadPool_.addFuture([this, doc](){
    return dbHandle_->saveDocument(doc);
  });
}

Future<ProcessedDocument*> DocumentDB::loadDocument(const string &docId) {
  return threadPool_.addFuture([this, docId](){
    return dbHandle_->loadDocument(docId);
  });
}

} // persistence
