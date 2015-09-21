#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "DocumentDBHandle.h"
#include "RockHandle.h"
#include "ProcessedDocument.h"

namespace {
  using namespace std;
  using namespace folly;
  using namespace wangle;
}

namespace persistence {

class DocumentDB {
protected:
  DocumentDBHandle *dbHandle_ {nullptr};
  FutureExecutor<CPUThreadPoolExecutor> threadPool_ {1};
  DocumentDB(){
    threadPool_.addFuture([this](){
      auto rock = std::make_unique<RockHandle>("data/documents");
      dbHandle_ = new DocumentDBHandle(std::move(rock));
    });
  }
  DocumentDB(DocumentDB const&) = delete;
  void operator=(DocumentDB const&) = delete;
public:

  static DocumentDB* getInstance() {
    static DocumentDB instance;
    return &instance;
  }

  Future<bool> doesDocumentExist(const string &docId) {
    return threadPool_.addFuture([this, docId](){
      return dbHandle_->doesDocumentExist(docId);
    });
  }

  Future<bool> deleteDocument(const string &docId) {
    return threadPool_.addFuture([this, docId](){
      return dbHandle_->deleteDocument(docId);
    });
  }

  Future<bool> saveDocument(ProcessedDocument *doc) {
    return threadPool_.addFuture([this, doc](){
      return dbHandle_->saveDocument(doc);
    });
  }

  Future<ProcessedDocument*> loadDocument(const string &docId) {
    return threadPool_.addFuture([this, docId](){
      return dbHandle_->loadDocument(docId);
    });
  }

};

} // persistence
