#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <functional>
#include <folly/Format.h>
#include <folly/Optional.h>
#include <glog/logging.h>
#include <rocksdb/db.h>
#include "RockHandle.h"
#include "DocumentDBHandle.h"
#include "ProcessedDocument.h"
#include "serialization/serializers.h"
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
  unsigned char *serialized;
  size_t len = serialization::serialize(&serialized, *doc);
  rocksdb::Slice data((char*) serialized, len);
  rockHandle_->put(doc->id, data);
  free(serialized);
  return true;
}

bool DocumentDBHandle::saveDocument(shared_ptr<ProcessedDocument> doc) {
  return saveDocument(doc.get());
}

bool DocumentDBHandle::saveDocument(UniquePointer<ProcessedDocument> doc) {
  return saveDocument(doc.get());
}

bool DocumentDBHandle::deleteDocument(const string &docId) {
  return rockHandle_->del(docId);
}

ProcessedDocument* DocumentDBHandle::loadDocumentDangerously(const string &docId) {
  string serialized;
  if (!rockHandle_->get(docId, serialized)) {
    return nullptr;
  }
  auto processed = new ProcessedDocument("");
  serialization::deserialize((unsigned char*) serialized.c_str(), *processed);
  return processed;
}

Optional<UniquePointer<ProcessedDocument>> DocumentDBHandle::loadDocument(const string &docId) {
  Optional<UniquePointer<ProcessedDocument>> result;
  auto ptr = loadDocumentDangerously(docId);
  if (ptr != nullptr) {
    util::UniquePointer<ProcessedDocument> processed(ptr);
    result.assign(std::move(processed));
  }
  return result;
}

Optional<shared_ptr<ProcessedDocument>> DocumentDBHandle::loadDocumentShared(const string &docId) {
  Optional<shared_ptr<ProcessedDocument>> result;
  auto ptr = loadDocumentDangerously(docId);
  if (ptr != nullptr) {
    shared_ptr<ProcessedDocument> processed(ptr);
    result.assign(processed);
  }
  return result;
}

vector<string> DocumentDBHandle::listDocuments() {
  vector<string> output;
  rockHandle_->iterAll([&output](rocksdb::Iterator *it, function<void()> escape) {
    output.push_back(it->key().ToString());
  });
  return output;
}

} // persistence
