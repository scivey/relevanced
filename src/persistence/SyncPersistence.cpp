#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"


#include <memory>
#include <string>
#include <vector>

#include <glog/logging.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/ExceptionWrapper.h>
#include <folly/Format.h>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <rocksdb/db.h>

#include "models/Centroid.h"
#include "models/ProcessedDocument.h"
#include "models/WordVector.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/RockHandle.h"
#include "serialization/serializers.h"
#include "util/util.h"

namespace relevanced {
namespace persistence {

using namespace std;
using namespace folly;
using models::ProcessedDocument;
using models::Centroid;

using namespace thrift_protocol;

SyncPersistence::SyncPersistence(util::UniquePointer<RockHandleIf> rockHandle)
    : rockHandle_(std::move(rockHandle)) {}

bool SyncPersistence::doesDocumentExist(const string &id) {
  auto key = sformat("documents:{}", id);
  return rockHandle_->exists(key);
}

Try<bool> SyncPersistence::saveNewDocument(ProcessedDocument *doc) {
  if (doesDocumentExist(doc->id)) {
    return Try<bool>(make_exception_wrapper<EDocumentAlreadyExists>());
  }
  auto key = sformat("documents:{}", doc->id);
  string data;
  serialization::binarySerialize(data, *doc);
  rockHandle_->put(key, data);
  return Try<bool>(true);
}

Try<bool> SyncPersistence::saveNewDocument(shared_ptr<ProcessedDocument> doc) {
  return saveNewDocument(doc.get());
}

Try<bool> SyncPersistence::saveDocument(ProcessedDocument *doc) {
  string data;
  serialization::binarySerialize(data, *doc);
  auto key = sformat("documents:{}", doc->id);
  rockHandle_->put(key, data);
  return Try<bool>(true);
}

Try<bool> SyncPersistence::saveDocument(shared_ptr<ProcessedDocument> doc) {
  return saveDocument(doc.get());
}

Try<bool> SyncPersistence::deleteDocument(const string &id) {
  auto key = sformat("documents:{}", id);
  if (!rockHandle_->exists(key)) {
    return Try<bool>(make_exception_wrapper<EDocumentDoesNotExist>());
  }
  bool rc = rockHandle_->del(key);
  return Try<bool>(rc);
}

vector<string> SyncPersistence::listAllDocuments() {
  vector<string> docIds;
  rockHandle_->iterPrefix("documents",
                          [&docIds](const string &key,
                                    function<void(string &) >,
                                    function<void()>) {
                            auto offset = key.find(':');
                            DCHECK(offset != string::npos);
                            docIds.push_back(key.substr(offset + 1));
                          });
  return docIds;
}

vector<string> SyncPersistence::listDocumentRangeFromId(
    const string &startingDocumentId, size_t numToGet) {
  vector<string> docIds;
  rockHandle_->iterPrefixFromMember("documents",
                                    startingDocumentId,
                                    numToGet,
                                    [&docIds](const string &key,
                                              function<void(string &) >,
                                              function<void()>) {
                                      auto offset = key.find(':');
                                      DCHECK(offset != string::npos);
                                      docIds.push_back(key.substr(offset + 1));
                                    });
  return docIds;
}

vector<string> SyncPersistence::listDocumentRangeFromOffset(size_t offset,
                                                            size_t numToGet) {
  vector<string> docIds;
  rockHandle_->iterPrefixFromOffset("documents",
                                    offset,
                                    numToGet,
                                    [&docIds](const string &key,
                                              function<void(string &) >,
                                              function<void()>) {
                                      auto offset = key.find(':');
                                      DCHECK(offset != string::npos);
                                      docIds.push_back(key.substr(offset + 1));
                                    });
  return docIds;
}

Optional<ProcessedDocument *> SyncPersistence::loadDocumentRaw(
    const string &docId) {
  Optional<ProcessedDocument *> result;
  auto key = sformat("documents:{}", docId);
  string serialized;
  if (!rockHandle_->get(key, serialized)) {
    return result;
  }
  auto processed = new ProcessedDocument("");
  serialization::binaryDeserialize(serialized, processed);
  result.assign(processed);
  return result;
}

Try<shared_ptr<ProcessedDocument>> SyncPersistence::loadDocument(
    const string &docId) {
  auto loadedOpt = loadDocumentRaw(docId);
  if (loadedOpt.hasValue()) {
    return Try<shared_ptr<ProcessedDocument>>(
        shared_ptr<ProcessedDocument>(loadedOpt.value()));
  }
  return Try<shared_ptr<ProcessedDocument>>(
      make_exception_wrapper<EDocumentDoesNotExist>());
}

Optional<shared_ptr<ProcessedDocument>> SyncPersistence::loadDocumentOption(
    const string &docId) {
  Optional<shared_ptr<ProcessedDocument>> result;
  auto loadedOpt = loadDocumentRaw(docId);
  if (loadedOpt.hasValue()) {
    result.assign(shared_ptr<ProcessedDocument>(loadedOpt.value()));
  }
  return result;
}

bool SyncPersistence::doesCentroidExist(const string &id) {
  auto key = sformat("centroids:{}", id);
  return (rockHandle_->exists(key));
}

Try<bool> SyncPersistence::createNewCentroid(const string &id) {
  auto key = sformat("centroids:{}", id);
  if (rockHandle_->exists(key)) {
    return Try<bool>(make_exception_wrapper<ECentroidAlreadyExists>());
  }
  Centroid centroid(id);
  saveCentroid(id, &centroid);
  return Try<bool>(true);
}

Try<bool> SyncPersistence::deleteCentroid(const string &id) {
  auto mainKey = sformat("centroids:{}", id);
  if (!rockHandle_->exists(mainKey)) {
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  rockHandle_->del(mainKey);
  vector<string> associatedDocuments;
  auto docPrefix = sformat("{}__documents", id);
  rockHandle_->iterPrefix(
      docPrefix,
      [&associatedDocuments](
          const string &key, function<void(string &) >, function<void()>) {
        auto offset = key.find(':');
        DCHECK(offset != string::npos);
        associatedDocuments.push_back(key.substr(offset + 1));
      });
  for (auto &docId : associatedDocuments) {
    auto key = sformat("{}__documents:{}", id, docId);
    rockHandle_->del(key);
  }
  return Try<bool>(true);
}

Try<bool> SyncPersistence::saveCentroid(const string &id, Centroid *centroid) {
  LOG(INFO) << format("serializing centroid '{}'", id);
  auto key = sformat("centroids:{}", id);
  LOG(INFO) << format("persisting serialized centroid '{}'", id);
  string data;
  serialization::binarySerialize(data, *centroid);
  rockHandle_->put(key, data);
  return Try<bool>(true);
}

Try<bool> SyncPersistence::saveCentroid(const string &id,
                                        shared_ptr<Centroid> centroid) {
  return saveCentroid(id, centroid.get());
}

Optional<Centroid *> SyncPersistence::loadCentroidRaw(const string &id) {
  Optional<Centroid *> result;
  string serialized;
  auto key = sformat("centroids:{}", id);
  if (rockHandle_->get(key, serialized)) {
    auto centroidRes = new Centroid;
    serialization::binaryDeserialize(serialized, centroidRes);
    result.assign(centroidRes);
  }
  return result;
}

Try<shared_ptr<Centroid>> SyncPersistence::loadCentroid(const string &id) {
  auto loadedRaw = loadCentroidRaw(id);
  if (!loadedRaw.hasValue()) {
    return Try<shared_ptr<Centroid>>(
        make_exception_wrapper<ECentroidDoesNotExist>());
  }
  return Try<shared_ptr<Centroid>>(shared_ptr<Centroid>(loadedRaw.value()));
}

Optional<shared_ptr<Centroid>> SyncPersistence::loadCentroidOption(
    const string &id) {
  Optional<shared_ptr<Centroid>> result;
  auto loadedRaw = loadCentroidRaw(id);
  if (loadedRaw.hasValue()) {
    result.assign(shared_ptr<Centroid>(loadedRaw.value()));
  }
  return result;
}

Optional<util::UniquePointer<Centroid>> SyncPersistence::loadCentroidUniqueOption(
    const string &id) {
  Optional<util::UniquePointer<Centroid>> result;
  auto loadedRaw = loadCentroidRaw(id);
  if (loadedRaw.hasValue()) {
    result.assign(std::move(util::UniquePointer<Centroid>(loadedRaw.value())));
  }
  return result;
}


vector<string> SyncPersistence::listAllCentroids() {
  string prefix = "centroids";
  vector<string> centroidIds;
  rockHandle_->iterPrefix(prefix,
                          [&centroidIds](const string &key,
                                         function<void(string &) >,
                                         function<void()>) {
                            auto offset = key.find(':');
                            DCHECK(offset != string::npos);
                            centroidIds.push_back(key.substr(offset + 1));
                          });
  return centroidIds;
}

vector<string> SyncPersistence::listCentroidRangeFromOffset(size_t offset,
                                                            size_t limit) {
  string prefix = "centroids";
  vector<string> centroidIds;
  rockHandle_->iterPrefixFromOffset(
      prefix,
      offset,
      limit,
      [&centroidIds](
          const string &key, function<void(string &) >, function<void()>) {
        auto offset = key.find(':');
        DCHECK(offset != string::npos);
        centroidIds.push_back(key.substr(offset + 1));
      });
  return centroidIds;
}

vector<string> SyncPersistence::listCentroidRangeFromId(
    const string &startingCentroidId, size_t limit) {
  string prefix = "centroids";
  vector<string> centroidIds;
  rockHandle_->iterPrefixFromMember(
      prefix,
      startingCentroidId,
      limit,
      [&centroidIds](
          const string &key, function<void(string &) >, function<void()>) {
        auto offset = key.find(':');
        DCHECK(offset != string::npos);
        centroidIds.push_back(key.substr(offset + 1));
      });
  return centroidIds;
}

Try<bool> SyncPersistence::addDocumentToCentroid(const string &centroidId,
                                                 const string &documentId) {
  if (!doesCentroidExist(centroidId)) {
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  if (!doesDocumentExist(documentId)) {
    return Try<bool>(make_exception_wrapper<EDocumentDoesNotExist>());
  }
  auto key = sformat("{}__documents:{}", centroidId, documentId);
  string val = "1";
  rockHandle_->put(key, val);
  return Try<bool>(true);
}

Try<bool> SyncPersistence::removeDocumentFromCentroid(
    const string &centroidId, const string &documentId) {
  if (!doesCentroidExist(centroidId)) {
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  if (!doesDocumentExist(documentId)) {
    return Try<bool>(make_exception_wrapper<EDocumentDoesNotExist>());
  }
  auto key = sformat("{}__documents:{}", centroidId, documentId);
  rockHandle_->del(key);
  return Try<bool>(true);
}

Try<bool> SyncPersistence::doesCentroidHaveDocument(const string &centroidId,
                                                    const string &documentId) {
  auto key = sformat("{}__documents:{}", centroidId, documentId);
  if (rockHandle_->exists(key)) {
    return Try<bool>(true);
  }
  if (!doesCentroidExist(centroidId)) {
    return Try<bool>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  return Try<bool>(false);
}

vector<string> SyncPersistence::listAllDocumentsForCentroidRaw(
    const string &centroidId) {
  vector<string> documentIds;
  auto prefix = sformat("{}__documents", centroidId);
  rockHandle_->iterPrefix(prefix,
                          [&documentIds](const string &key,
                                         function<void(string &) >,
                                         function<void()>) {
                            auto offset = key.find(':');
                            DCHECK(offset != string::npos);
                            documentIds.push_back(key.substr(offset + 1));
                          });
  return documentIds;
}

Try<vector<string>> SyncPersistence::listAllDocumentsForCentroid(
    const string &centroidId) {
  auto docs = listAllDocumentsForCentroidRaw(centroidId);
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return Try<vector<string>>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  return Try<vector<string>>(docs);
}

Optional<vector<string>> SyncPersistence::listAllDocumentsForCentroidOption(
    const string &centroidId) {
  Optional<vector<string>> result;
  auto docs = listAllDocumentsForCentroidRaw(centroidId);
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return result;
  }
  result.assign(docs);
  return result;
}

vector<string> SyncPersistence::listCentroidDocumentRangeFromOffsetRaw(
    const string &centroidId, size_t offset, size_t limit) {
  vector<string> documentIds;
  auto prefix = sformat("{}__documents", centroidId);
  rockHandle_->iterPrefixFromOffset(
      prefix,
      offset,
      limit,
      [&documentIds](
          const string &key, function<void(string &) >, function<void()>) {
        auto offset = key.find(':');
        DCHECK(offset != string::npos);
        documentIds.push_back(key.substr(offset + 1));
      });
  return documentIds;
}

Optional<vector<string>>
SyncPersistence::listCentroidDocumentRangeFromOffsetOption(
    const string &centroidId, size_t offset, size_t limit) {
  Optional<vector<string>> result;
  auto docs = listCentroidDocumentRangeFromOffsetRaw(centroidId, offset, limit);
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return result;
  }
  result.assign(docs);
  return result;
}

Try<vector<string>> SyncPersistence::listCentroidDocumentRangeFromOffset(
    const string &centroidId, size_t offset, size_t limit) {
  Optional<vector<string>> result;
  auto docs = listCentroidDocumentRangeFromOffsetRaw(centroidId, offset, limit);
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return Try<vector<string>>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  return Try<vector<string>>(docs);
}

vector<string> SyncPersistence::listCentroidDocumentRangeFromDocumentIdRaw(
    const string &centroidId, const string &startingDocumentId, size_t limit) {
  vector<string> documentIds;
  auto prefix = sformat("{}__documents", centroidId);
  rockHandle_->iterPrefixFromMember(
      prefix,
      startingDocumentId,
      limit,
      [&documentIds](
          const string &key, function<void(string &) >, function<void()>) {
        auto offset = key.find(':');
        DCHECK(offset != string::npos);
        documentIds.push_back(key.substr(offset + 1));
      });
  return documentIds;
}

Optional<vector<string>>
SyncPersistence::listCentroidDocumentRangeFromDocumentIdOption(
    const string &centroidId, const string &documentId, size_t limit) {
  Optional<vector<string>> result;
  auto docs =
      listCentroidDocumentRangeFromDocumentIdRaw(centroidId, documentId, limit);
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return result;
  }
  result.assign(docs);
  return result;
}

Try<vector<string>> SyncPersistence::listCentroidDocumentRangeFromDocumentId(
    const string &centroidId, const string &documentId, size_t limit) {
  Optional<vector<string>> result;
  auto docs =
      listCentroidDocumentRangeFromDocumentIdRaw(centroidId, documentId, limit);
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return Try<vector<string>>(make_exception_wrapper<ECentroidDoesNotExist>());
  }
  return Try<vector<string>>(docs);
}


Optional<string> SyncPersistence::getCentroidMetadata(
    const string &centroidId, const string &metadataName) {
  auto key = sformat("{}__centroid_metadata:{}", centroidId, metadataName);
  Optional<string> result;
  string keyVal;
  if (rockHandle_->get(key, keyVal)) {
    result.assign(keyVal);
  }
  return result;
}

Try<bool> SyncPersistence::setCentroidMetadata(const string &centroidId,
                                               const string &metadataName,
                                               string value) {
  auto key = sformat("{}__centroid_metadata:{}", centroidId, metadataName);
  rockHandle_->put(key, value);
  return Try<bool>(true);
}

void SyncPersistence::debugEraseAllData() {
  rockHandle_->eraseEverything();
}

} // persistence
} // relevanced
