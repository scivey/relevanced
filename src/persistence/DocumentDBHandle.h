#pragma once
#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <folly/Optional.h>
#include <glog/logging.h>
#include "RockHandle.h"
#include "ProcessedDocument.h"
#include "util.h"

namespace persistence {

class DocumentDBHandleIf {
public:
  virtual bool doesDocumentExist(const std::string &docId) = 0;
  virtual bool saveDocument(ProcessedDocument *doc) = 0;
  virtual bool saveDocument(util::UniquePointer<ProcessedDocument> doc) = 0;
  virtual bool saveDocument(std::shared_ptr<ProcessedDocument> doc) = 0;
  virtual bool deleteDocument(const std::string &docId) = 0;
  virtual std::vector<std::string> listDocuments() = 0;
  virtual folly::Optional<util::UniquePointer<ProcessedDocument>> loadDocument(const std::string &docId) = 0;
  virtual folly::Optional<std::shared_ptr<ProcessedDocument>> loadDocumentShared(const std::string &docId) = 0;
  virtual ~DocumentDBHandleIf() = default;
};

class DocumentDBHandle: public DocumentDBHandleIf {
protected:
  util::UniquePointer<RockHandleIf> rockHandle_;
  ProcessedDocument* loadDocumentDangerously(const string &docId);
public:
  DocumentDBHandle(util::UniquePointer<RockHandleIf> rockHandle);
  bool doesDocumentExist(const std::string &docId) override;
  bool saveDocument(ProcessedDocument *doc) override;
  bool saveDocument(util::UniquePointer<ProcessedDocument> doc) override;
  bool saveDocument(std::shared_ptr<ProcessedDocument> doc) override;
  bool deleteDocument(const std::string &docId) override;
  std::vector<std::string> listDocuments() override;
  folly::Optional<util::UniquePointer<ProcessedDocument>> loadDocument(const std::string &docId) override;
  folly::Optional<std::shared_ptr<ProcessedDocument>> loadDocumentShared(const std::string &docId) override;

};

} // persistence
