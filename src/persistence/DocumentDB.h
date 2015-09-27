#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/Optional.h>

#include "DocumentDBHandle.h"

#include "RockHandle.h"
#include "ProcessedDocument.h"
#include "util.h"

namespace persistence {

class DocumentDBIf {
public:
  virtual void initialize() = 0;
  virtual folly::Future<bool> doesDocumentExist(const std::string &docId) = 0;
  virtual folly::Future<bool> deleteDocument(const std::string &docId) = 0;
  virtual folly::Future<bool> saveDocument(ProcessedDocument *doc) = 0;
  virtual folly::Future<bool> saveDocument(std::shared_ptr<ProcessedDocument> doc) = 0;
  virtual folly::Future<folly::Optional<util::UniquePointer<ProcessedDocument>>> loadDocument(const std::string &docId) = 0;
  virtual folly::Future<folly::Optional<std::shared_ptr<ProcessedDocument>>> loadDocumentShared(const std::string &docId) = 0;
  virtual folly::Future<std::vector<std::string>> listDocuments() = 0;
  virtual ~DocumentDBIf() = default;
};

class DocumentDB: public DocumentDBIf {
protected:
  util::UniquePointer<DocumentDBHandleIf> dbHandle_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_ ;
  DocumentDB(DocumentDB const&) = delete;
  void operator=(DocumentDB const&) = delete;
public:
  DocumentDB(
    util::UniquePointer<DocumentDBHandleIf>,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>>
  );
  void initialize() override;
  folly::Future<bool> doesDocumentExist(const std::string &docId) override;
  folly::Future<bool> deleteDocument(const std::string &docId) override;
  folly::Future<bool> saveDocument(ProcessedDocument *doc) override;
  folly::Future<bool> saveDocument(std::shared_ptr<ProcessedDocument> doc) override;
  folly::Future<folly::Optional<util::UniquePointer<ProcessedDocument>>> loadDocument(const std::string &docId) override;
  folly::Future<folly::Optional<std::shared_ptr<ProcessedDocument>>> loadDocumentShared(const std::string &docId) override;
  folly::Future<std::vector<std::string>> listDocuments() override;
};

} // persistence
