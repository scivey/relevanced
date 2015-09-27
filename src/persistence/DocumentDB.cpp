#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/Optional.h>
#include "DocumentDB.h"
#include "DocumentDBHandle.h"
#include "RockHandle.h"
#include "ProcessedDocument.h"
#include "util.h"
using namespace std;
using namespace folly;
using namespace wangle;
using util::UniquePointer;
namespace persistence {

DocumentDB::DocumentDB(UniquePointer<DocumentDBHandleIf> dbHandle, shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
  : dbHandle_(std::move(dbHandle)), threadPool_(threadPool){}

void DocumentDB::initialize() {}

Future<bool> DocumentDB::doesDocumentExist(const string &docId) {
  return threadPool_->addFuture([this, docId](){
    return dbHandle_->doesDocumentExist(docId);
  });
}

Future<bool> DocumentDB::deleteDocument(const string &docId) {
  threadPool_->addFuture([this, docId](){
    dbHandle_->deleteDocument(docId);
  });
  return makeFuture(true);
}

Future<bool> DocumentDB::saveDocument(ProcessedDocument *doc) {
  threadPool_->addFuture([this, doc](){
    dbHandle_->saveDocument(doc);
  });
  return makeFuture(true);
}

Future<bool> DocumentDB::saveDocument(shared_ptr<ProcessedDocument> doc) {
  threadPool_->addFuture([this, doc](){
    dbHandle_->saveDocument(doc);
  });
  return makeFuture(true);
}

Future<Optional<UniquePointer<ProcessedDocument>>> DocumentDB::loadDocument(const string &docId) {
  return threadPool_->addFuture([this, docId](){
    return std::move(dbHandle_->loadDocument(docId));
  });
}

Future<Optional<shared_ptr<ProcessedDocument>>> DocumentDB::loadDocumentShared(const string &docId) {
  return threadPool_->addFuture([this, docId](){
    return dbHandle_->loadDocumentShared(docId);
  });
}

Future<vector<string>> DocumentDB::listDocuments() {
  return threadPool_->addFuture([this](){
    return dbHandle_->listDocuments();
  });
}

} // persistence
