#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>
#include "RockHandle.h"
#include "DocumentDBHandle.h"
#include "ProcessedDocument.h"
#include "util.h"
using namespace std;
using namespace folly;
using util::UniquePointer;

namespace persistence {

DocumentDBHandle::DocumentDBHandle(UniquePointer<RockHandleIf> rockHandle)
  : rockHandle_(std::move(rockHandle)) {}

bool DocumentDBHandle::doesDocumentExist(const string &docId) {
  return rockHandle_->exists(docId);
}

bool DocumentDBHandle::saveDocument(ProcessedDocument *doc) {
  auto val = doc->toJson();
  rockHandle_->put(doc->id, val);
  return true;
}

bool DocumentDBHandle::deleteDocument(const string &docId) {
  return rockHandle_->del(docId);
}

ProcessedDocument* DocumentDBHandle::loadDocument(const string &docId) {
  auto serialized = rockHandle_->get(docId);
  return ProcessedDocument::newFromJson(serialized);
}

} // persistence
