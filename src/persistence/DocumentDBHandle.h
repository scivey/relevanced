#pragma once
#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>
#include "RockHandle.h"
#include "ProcessedDocument.h"
#include "util.h"
namespace persistence {

class DocumentDBHandleIf {
public:
  virtual bool doesDocumentExist(const std::string &docId) = 0;
  virtual bool saveDocument(ProcessedDocument *doc) = 0;
  virtual bool deleteDocument(const std::string &docId) = 0;
  virtual ProcessedDocument* loadDocument(const std::string &docId) = 0;
  virtual ~DocumentDBHandleIf() = default;
};

class DocumentDBHandle: public DocumentDBHandleIf {
protected:
  util::UniquePointer<RockHandleIf> rockHandle_;
public:
  DocumentDBHandle(util::UniquePointer<RockHandleIf> rockHandle);
  bool doesDocumentExist(const std::string &docId) override;
  bool saveDocument(ProcessedDocument *doc) override;
  bool deleteDocument(const std::string &docId) override;
  ProcessedDocument* loadDocument(const std::string &docId) override;
};

} // persistence
