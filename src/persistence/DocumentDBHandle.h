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

namespace {
  using namespace std;
  using namespace folly;
}
namespace persistence {

class DocumentDBHandle {
protected:
  unique_ptr<RockHandle> rockHandle_;
public:
  DocumentDBHandle(unique_ptr<RockHandle> rockHandle)
    : rockHandle_(std::move(rockHandle)) {}

  bool doesDocumentExist(const string &docId) {
    return rockHandle_->exists(docId);
  }

  bool saveDocument(ProcessedDocument *doc) {
    auto val = doc->toJson();
    rockHandle_->put(doc->id, val);
    return true;
  }

  ProcessedDocument loadDocument(const string &docId) {
    auto serialized = rockHandle_->get(docId);
    ProcessedDocument doc = ProcessedDocument::fromJson(serialized);
    return doc;
  }

};

} // persistence
