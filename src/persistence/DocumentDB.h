#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>

#include "DocumentDBHandle.h"
#include "DocumentDBCache.h"

#include "RockHandle.h"
#include "ProcessedDocument.h"
#include "util.h"

namespace persistence {

class DocumentDBIf {
public:
  virtual folly::Future<bool> doesDocumentExist(const std::string &docId) = 0;
  virtual folly::Future<bool> deleteDocument(const std::string &docId) = 0;
  virtual folly::Future<bool> saveDocument(ProcessedDocument *doc) = 0;
  virtual folly::Future<ProcessedDocument*> loadDocument(const std::string &docId) = 0;
  virtual ~DocumentDBIf() = default;
};

class DocumentDB: public DocumentDBIf {
protected:
  util::UniquePointer<DocumentDBHandleIf> dbHandle_;
  std::shared_ptr<DocumentDBCache> dbCache_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_ ;
  DocumentDB(DocumentDB const&) = delete;
  void operator=(DocumentDB const&) = delete;
public:
  DocumentDB(
    util::UniquePointer<DocumentDBHandleIf>,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
  );
  folly::Future<bool> doesDocumentExist(const std::string &docId) override;
  folly::Future<bool> deleteDocument(const std::string &docId) override;
  folly::Future<bool> saveDocument(ProcessedDocument *doc) override;
  folly::Future<ProcessedDocument*> loadDocument(const std::string &docId) override;
};

} // persistence
