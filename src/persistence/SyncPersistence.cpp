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
#include "util/Clock.h"


namespace relevanced {
namespace persistence {

using namespace std;
using namespace folly;
using models::ProcessedDocument;
using models::Centroid;
using namespace thrift_protocol;
using util::ClockIf;
using util::UniquePointer;


SyncPersistence::SyncPersistence(
  shared_ptr<ClockIf> clockPtr,
  UniquePointer<RockHandleIf> rockHandle
) : clock_(clockPtr), rockHandle_(std::move(rockHandle)) {}


string SyncPersistence::getCentroidsPrefix() {
  return "centroids";
}


string SyncPersistence::getDocumentsPrefix() {
  return "documents";
}


string SyncPersistence::getCentroidKey(
    const string &id) {
  return sformat(
    "{}:{}",
    SyncPersistence::getCentroidsPrefix(),
    id
  );
}


string SyncPersistence::getDocumentKey(
    const string &id) {
  return sformat(
    "{}:{}",
    SyncPersistence::getDocumentsPrefix(),
    id
  );
}


string SyncPersistence::getCentroidDocumentPrefix(
    const string &id) {
  return sformat("{}__documents", id);
}


string SyncPersistence::getDocumentCentroidsPrefix(
    const string &docId) {
  return sformat("{}__centroids", docId);
}


string SyncPersistence::getDocumentCentroidKey(
    const string &docId, const string &centroidId) {
  return sformat("{}__centroids:{}", docId, centroidId);
}


string SyncPersistence::getCentroidDocumentKey(
    const string &centroidId, const string &docId) {
  return sformat("{}__documents:{}", centroidId, docId);
}


string SyncPersistence::getCentroidMetadataKey(
    const string &id, const string &metaName) {
  return sformat("{}__centroid_metadata:{}", id, metaName);
}


string SyncPersistence::getDocumentMetadataKey(
    const string &id, const string &metaName) {
  return sformat("{}__document_metadata:{}", id, metaName);
}


Optional<int64_t> SyncPersistence::getDocumentCreatedTime(
    const string &id) {
  auto key = getDocumentMetadataKey(
    id, "created_time"
  );
  string data;
  Optional<int64_t> result;
  if (rockHandle_->get(key, data)) {
    int64_t createdTime = folly::to<int64_t>(data);
    result.assign(createdTime);
  }
  return result;
}


void SyncPersistence::setDocumentCreatedTime(
    const string &id, int64_t created) {
  auto key = getDocumentMetadataKey(id, "created_time");
  string data = folly::to<string>(created);
  rockHandle_->put(key, data);
}

bool SyncPersistence::isDocumentInAnyCentroid(
    const string &documentId) {
  bool iterated = rockHandle_->iterPrefix(
    getDocumentCentroidsPrefix(documentId),
    [](const string&,
        function<void (string&)>,
        function<void()> escape) {
      escape();
    });
  return iterated;
}


vector<string> SyncPersistence::listDocumentCentroids(
    const string &documentId) {
  vector<string> centroids;
  rockHandle_->iterPrefix(
    getDocumentCentroidsPrefix(documentId),
    [&centroids](const string &key,
        function<void(string&)>,
        function<void()>) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      centroids.push_back(key.substr(offset + 1));
    });
  return centroids;
}


bool SyncPersistence::doesDocumentExist(const string &id) {
  auto key = SyncPersistence::getDocumentKey(id);
  return rockHandle_->exists(key);
}


Try<bool> SyncPersistence::saveNewDocument(ProcessedDocument *doc) {
  if (doesDocumentExist(doc->id)) {
    return Try<bool>(
      make_exception_wrapper<EDocumentAlreadyExists>()
    );
  }
  return saveDocument(doc);
}

Try<bool> SyncPersistence::saveNewDocument(
    shared_ptr<ProcessedDocument> doc) {
  return saveNewDocument(doc.get());
}


Try<bool> SyncPersistence::saveDocument(ProcessedDocument *doc) {
  string data;
  serialization::binarySerialize(data, *doc);
  rockHandle_->put(
    SyncPersistence::getDocumentKey(doc->id), data
  );
  setDocumentCreatedTime(doc->id, doc->created);
  return Try<bool>(true);
}


Try<bool> SyncPersistence::saveDocument(
    shared_ptr<ProcessedDocument> doc) {
  return saveDocument(doc.get());
}


Try<bool> SyncPersistence::deleteDocument(const string &id) {
  auto mainKey = SyncPersistence::getDocumentKey(id);
  if (rockHandle_->del(mainKey)) {
    rockHandle_->iterPrefix(
      SyncPersistence::getDocumentCentroidsPrefix(id),
      [this](const string &key,
          function<void(string&)>,
          function<void()>) {
        rockHandle_->del(key);
      });
    rockHandle_->iterPrefix(
      sformat("{}__document_metadata", id),
      [this](const string &key,
          function<void(string&)>,
          function<void()>) {
        rockHandle_->del(key);
      });
    return Try<bool>(true);
  }
  return Try<bool>(
    make_exception_wrapper<EDocumentDoesNotExist>()
  );
}


vector<string> SyncPersistence::listUnusedDocuments(
    size_t limit = 0) {
  vector<string> docIds;
  size_t numSeen = 0;
  rockHandle_->iterPrefix(
    SyncPersistence::getDocumentsPrefix(),
    [this, &docIds, &numSeen, limit]
    (const string &key,
        function<void(string &)>,
        function<void()> escape) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      auto id = key.substr(offset + 1);
      if (!isDocumentInAnyCentroid(id)) {
        docIds.push_back(id);
        if (limit > 0) {
          numSeen++;
          if (numSeen >= limit) {
            escape();
          }
        }
      }
    });
  return docIds;
}


size_t SyncPersistence::deleteOldUnusedDocuments(
    int64_t minAge = 3600, size_t limit = 0) {

  auto startTime = clock_->getEpochTime();
  auto cutoff = startTime - minAge;
  size_t numSeen = 0;
  rockHandle_->iterPrefix(
    SyncPersistence::getDocumentsPrefix(),
    [this, &numSeen, cutoff, limit]
    (const string &key,
        function<void(string &)>,
        function<void()> escape) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      auto id = key.substr(offset + 1);
      if (!isDocumentInAnyCentroid(id)) {
        auto createdTime = getDocumentCreatedTime(id);
        if (!createdTime.hasValue() ||
              createdTime.value() < cutoff) {
          deleteDocument(id);
          if (limit > 0) {
            numSeen++;
            if (numSeen >= limit) {
              escape();
            }
          }
        }
      }
    });
  return numSeen;
}


vector<string> SyncPersistence::listAllDocuments() {
  vector<string> docIds;
  rockHandle_->iterPrefix(
    SyncPersistence::getDocumentsPrefix(),
    [&docIds](const string &key,
        function<void(string &)>,
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
  rockHandle_->iterPrefixFromMember(
    SyncPersistence::getDocumentsPrefix(),
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


vector<string> SyncPersistence::listDocumentRangeFromOffset(
    size_t offset, size_t numToGet) {
  vector<string> docIds;
  rockHandle_->iterPrefixFromOffset(
    SyncPersistence::getDocumentsPrefix(),
    offset,
    numToGet,
    [&docIds](const string &key,
        function<void(string &)>,
        function<void()>) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      docIds.push_back(key.substr(offset + 1));
    });
  return docIds;
}

Try<shared_ptr<ProcessedDocument>> SyncPersistence::loadDocument(
    const string &docId) {
  string key = SyncPersistence::getDocumentKey(docId);
  string serialized;
  if (!rockHandle_->get(key, serialized)) {
    return Try<shared_ptr<ProcessedDocument>>(
      make_exception_wrapper<EDocumentDoesNotExist>()
    );
  }
  auto processed = std::make_shared<ProcessedDocument>();
  serialization::binaryDeserialize(serialized, processed.get());
  return Try<shared_ptr<ProcessedDocument>>(std::move(processed));
}


bool SyncPersistence::doesCentroidExist(const string &id) {
  auto key = SyncPersistence::getCentroidKey(id);
  return (rockHandle_->exists(key));
}


Try<bool> SyncPersistence::createNewCentroid(const string &id) {
  auto key = SyncPersistence::getCentroidKey(id);
  if (rockHandle_->exists(key)) {
    return Try<bool>(
      make_exception_wrapper<ECentroidAlreadyExists>()
    );
  }
  Centroid centroid(id);
  saveCentroid(id, &centroid);
  return Try<bool>(true);
}


Try<bool> SyncPersistence::deleteCentroid(const string &id) {
  auto mainKey = SyncPersistence::getCentroidKey(id);
  if (!rockHandle_->del(mainKey)) {
    return Try<bool>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  rockHandle_->iterPrefix(
    SyncPersistence::getCentroidDocumentPrefix(id),
    [this] (const string &key,
        function<void(string &)>,
        function<void()>) {
      rockHandle_->del(key);
    });
  rockHandle_->iterPrefix(
    sformat("{}__centroid_metadata", id),
    [this](const string &key,
        function<void(string&)>,
        function<void()>) {
      rockHandle_->del(key);
    });
  return Try<bool>(true);
}


Try<bool> SyncPersistence::saveCentroid(const string &id,
    Centroid *centroid) {
  string data;
  serialization::binarySerialize(data, *centroid);
  rockHandle_->put(SyncPersistence::getCentroidKey(id), data);
  return Try<bool>(true);
}


Try<bool> SyncPersistence::saveCentroid(const string &id,
    shared_ptr<Centroid> centroid) {
  return saveCentroid(id, centroid.get());
}

Try<shared_ptr<Centroid>> SyncPersistence::loadCentroid(
    const string &id) {
  string serialized;
  string key = SyncPersistence::getCentroidKey(id);
  if (!rockHandle_->get(key, serialized)) {
    return Try<shared_ptr<Centroid>>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  auto centroidRes = std::make_shared<Centroid>();
  serialization::binaryDeserialize(serialized, centroidRes.get());
  return Try<shared_ptr<Centroid>>(centroidRes);
}

Optional<util::UniquePointer<Centroid>> SyncPersistence::loadCentroidUniqueOption(
    const string &id) {
  string serialized;
  string key = SyncPersistence::getCentroidKey(id);
  Optional<util::UniquePointer<Centroid>> result;
  if (!rockHandle_->get(key, serialized)) {
    return std::move(result);
  }
  util::UniquePointer<Centroid> outPtr(new Centroid);
  serialization::binaryDeserialize(serialized, outPtr.get());
  result.assign(std::move(outPtr));
  return std::move(result);
}


vector<string> SyncPersistence::listAllCentroids() {
  vector<string> centroidIds;
  rockHandle_->iterPrefix(
    SyncPersistence::getCentroidsPrefix(),
    [&centroidIds](const string &key,
        function<void(string &)>,
        function<void()>) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      centroidIds.push_back(key.substr(offset + 1));
    });
  return centroidIds;
}


vector<string> SyncPersistence::listCentroidRangeFromOffset(
    size_t offset, size_t limit) {
  vector<string> centroidIds;
  rockHandle_->iterPrefixFromOffset(
    SyncPersistence::getCentroidsPrefix(),
    offset,
    limit,
    [&centroidIds](const string &key,
        function<void(string &) >,
        function<void()>) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      centroidIds.push_back(key.substr(offset + 1));
    });
  return centroidIds;
}


vector<string> SyncPersistence::listCentroidRangeFromId(
    const string &startingCentroidId, size_t limit) {
  vector<string> centroidIds;
  rockHandle_->iterPrefixFromMember(
    SyncPersistence::getCentroidsPrefix(),
    startingCentroidId,
    limit,
    [&centroidIds](const string &key,
        function<void(string &) >,
        function<void()>) {
      auto offset = key.find(':');
      DCHECK(offset != string::npos);
      centroidIds.push_back(key.substr(offset + 1));
    });
  return centroidIds;
}


Try<bool> SyncPersistence::addDocumentToCentroid(
    const string &centroidId, const string &documentId) {
  if (!doesCentroidExist(centroidId)) {
    return Try<bool>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  if (!doesDocumentExist(documentId)) {
    return Try<bool>(
      make_exception_wrapper<EDocumentDoesNotExist>()
    );
  }
  auto centroidKey = SyncPersistence::getCentroidDocumentKey(
    centroidId, documentId
  );
  if (rockHandle_->exists(centroidKey)) {
    return Try<bool>(
      make_exception_wrapper<EDocumentAlreadyInCentroid>()
    );
  }
  string val = "1";
  rockHandle_->put(centroidKey, val);
  auto documentKey = SyncPersistence::getDocumentCentroidKey(
    documentId, centroidId
  );
  rockHandle_->put(documentKey, val);
  return Try<bool>(true);
}


Try<bool> SyncPersistence::removeDocumentFromCentroid(
    const string &centroidId, const string &documentId) {
  if (!doesCentroidExist(centroidId)) {
    return Try<bool>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  if (!doesDocumentExist(documentId)) {
    return Try<bool>(
      make_exception_wrapper<EDocumentDoesNotExist>()
    );
  }
  auto centroidKey = SyncPersistence::getCentroidDocumentKey(
    centroidId, documentId
  );
  auto documentKey = SyncPersistence::getDocumentCentroidKey(
    documentId, centroidId
  );
  rockHandle_->del(documentKey);
  if (!rockHandle_->del(centroidKey)) {
    return Try<bool>(
      make_exception_wrapper<EDocumentNotInCentroid>()
    );
  }
  return Try<bool>(true);
}


Try<bool> SyncPersistence::doesCentroidHaveDocument(
    const string &centroidId, const string &documentId) {
  auto key = SyncPersistence::getCentroidDocumentKey(
    centroidId, documentId
  );
  if (rockHandle_->exists(key)) {
    return Try<bool>(true);
  }
  if (!doesCentroidExist(centroidId)) {
    return Try<bool>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  return Try<bool>(false);
}


vector<string> SyncPersistence::listAllDocumentsForCentroidRaw(
    const string &centroidId) {
  vector<string> documentIds;
  rockHandle_->iterPrefix(
    SyncPersistence::getCentroidDocumentPrefix(centroidId),
    [&documentIds](const string &key,
        function<void(string &)>,
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
    return Try<vector<string>>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
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
  rockHandle_->iterPrefixFromOffset(
    SyncPersistence::getCentroidDocumentPrefix(centroidId),
    offset,
    limit,
    [&documentIds](const string &key,
        function<void(string &) >,
        function<void()>) {
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
  auto docs = listCentroidDocumentRangeFromOffsetRaw(
    centroidId, offset, limit
  );
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return result;
  }
  result.assign(docs);
  return result;
}

Try<vector<string>> SyncPersistence::listCentroidDocumentRangeFromOffset(
    const string &centroidId, size_t offset, size_t limit) {
  Optional<vector<string>> result;
  auto docs = listCentroidDocumentRangeFromOffsetRaw(
    centroidId, offset, limit
  );
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return Try<vector<string>>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  return Try<vector<string>>(docs);
}

vector<string> SyncPersistence::listCentroidDocumentRangeFromDocumentIdRaw(
    const string &centroidId, const string &startingDocumentId, size_t limit) {
  vector<string> documentIds;
  rockHandle_->iterPrefixFromMember(
    SyncPersistence::getCentroidDocumentPrefix(centroidId),
    startingDocumentId,
    limit,
    [&documentIds](const string &key,
        function<void(string &)>,
        function<void()>) {
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
  auto docs = listCentroidDocumentRangeFromDocumentIdRaw(
    centroidId, documentId, limit
  );
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return result;
  }
  result.assign(docs);
  return result;
}

Try<vector<string>> SyncPersistence::listCentroidDocumentRangeFromDocumentId(
    const string &centroidId, const string &documentId, size_t limit) {
  Optional<vector<string>> result;
  auto docs = listCentroidDocumentRangeFromDocumentIdRaw(
    centroidId, documentId, limit
  );
  if (!docs.size() && !doesCentroidExist(centroidId)) {
    return Try<vector<string>>(
      make_exception_wrapper<ECentroidDoesNotExist>()
    );
  }
  return Try<vector<string>>(docs);
}


Optional<string> SyncPersistence::getCentroidMetadata(
    const string &centroidId, const string &metadataName) {
  auto key = SyncPersistence::getCentroidMetadataKey(
    centroidId, metadataName
  );
  Optional<string> result;
  string keyVal;
  if (rockHandle_->get(key, keyVal)) {
    result.assign(keyVal);
  }
  return result;
}

Try<bool> SyncPersistence::setCentroidMetadata(const string &centroidId,
    const string &metadataName, string value) {
  auto key = SyncPersistence::getCentroidMetadataKey(
    centroidId, metadataName
  );
  rockHandle_->put(key, value);
  return Try<bool>(true);
}

void SyncPersistence::debugEraseAllData() {
  rockHandle_->eraseEverything();
}

} // persistence
} // relevanced
