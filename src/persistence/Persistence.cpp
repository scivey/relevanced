#include <memory>
#include <string>
#include <vector>

#include <glog/logging.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>
#include <folly/Format.h>
#include <folly/Optional.h>

#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/CentroidMetadataDb.h"

#include "util/util.h"
#include "models/WordVector.h"
#include "models/Centroid.h"
#include "models/ProcessedDocument.h"


using namespace std;
using namespace folly;

namespace relevanced {
namespace persistence {
using wangle::FutureExecutor;
using wangle::CPUThreadPoolExecutor;
using models::WordVector;
using models::ProcessedDocument;
using models::Centroid;
using util::UniquePointer;

Persistence::Persistence(
  UniquePointer<SyncPersistenceIf> syncHandle,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
) : syncHandle_(std::move(syncHandle)), threadPool_(threadPool) {}

Future<bool> Persistence::doesDocumentExist(string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->doesDocumentExist(id);
  });
}

Future<Try<bool>> Persistence::saveDocument(
    shared_ptr<ProcessedDocument> doc) {
  return threadPool_->addFuture([this, doc]() {
    return syncHandle_->saveDocument(doc);
  });
}

Future<Try<bool>> Persistence::saveNewDocument(
    shared_ptr<ProcessedDocument> doc) {
  return threadPool_->addFuture([this, doc]() {
    return syncHandle_->saveNewDocument(doc);
  });
}

Future<Try<bool>> Persistence::deleteDocument(string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->deleteDocument(id);
  });
}

Future<vector<string>> Persistence::listAllDocuments() {
  return threadPool_->addFuture([this]() {
    return syncHandle_->listAllDocuments();
  });
}

Future<vector<string>> Persistence::listUnusedDocuments(size_t count) {
  return threadPool_->addFuture([this, count]() {
    return syncHandle_->listUnusedDocuments(count);
  });
}

Future<vector<string>> Persistence::listDocumentRangeFromId(
    string documentId, size_t count) {
  return threadPool_->addFuture([this, documentId, count]() {
    return syncHandle_->listDocumentRangeFromId(documentId, count);
  });
}

Future<vector<string>> Persistence::listDocumentRangeFromOffset(
    size_t offset, size_t count) {
  return threadPool_->addFuture([this, offset, count]() {
    return syncHandle_->listDocumentRangeFromOffset(offset, count);
  });
}

Future<Try<shared_ptr<ProcessedDocument>>> Persistence::loadDocument(
    string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->loadDocument(id);
  });
}

Future<bool> Persistence::doesCentroidExist(string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->doesCentroidExist(id);
  });
}

Future<Try<bool>> Persistence::createNewCentroid(string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->createNewCentroid(id);
  });
}

Future<Try<bool>> Persistence::deleteCentroid(string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->deleteCentroid(id);
  });
}

Future<Try<bool>> Persistence::saveCentroid(
    string id, shared_ptr<Centroid> centroid) {
  return threadPool_->addFuture([this, id, centroid]() {
    return syncHandle_->saveCentroid(id, centroid);
  });
}

Future<Try<shared_ptr<Centroid>>> Persistence::loadCentroid(
    string id) {
  return threadPool_->addFuture([this, id]() {
    return syncHandle_->loadCentroid(id);
  });
}

Future<Optional<util::UniquePointer<Centroid>>> Persistence::loadCentroidUniqueOption(
    string id) {
  return threadPool_->addFuture([this, id]() {
    return std::move(syncHandle_->loadCentroidUniqueOption(id));
  });
}

Future<vector<string>> Persistence::listAllCentroids() {
  return threadPool_->addFuture([this]() {
    return syncHandle_->listAllCentroids();
  });
}

Future<vector<string>> Persistence::listCentroidRangeFromOffset(
    size_t offset, size_t limit) {
  return threadPool_->addFuture([this, offset, limit]() {
    return syncHandle_->listCentroidRangeFromOffset(offset, limit);
  });
}

Future<vector<string>> Persistence::listCentroidRangeFromId(
    string startingCentroidId, size_t limit) {
  return threadPool_->addFuture([this, startingCentroidId, limit]() {
    return syncHandle_->listCentroidRangeFromId(startingCentroidId, limit);
  });
}

Future<Try<bool>> Persistence::addDocumentToCentroid(
    string centroidId, string docId) {
  return threadPool_->addFuture([this, centroidId, docId]() {
    return syncHandle_->addDocumentToCentroid(centroidId, docId);
  });
}

Future<Try<bool>> Persistence::removeDocumentFromCentroid(
    string centroidId, string docId) {
  return threadPool_->addFuture([this, centroidId, docId]() {
    return syncHandle_->removeDocumentFromCentroid(centroidId, docId);
  });
}

Future<Try<bool>> Persistence::doesCentroidHaveDocument(
    string centroidId, string docId) {
  return threadPool_->addFuture([this, centroidId, docId]() {
    return syncHandle_->doesCentroidHaveDocument(centroidId, docId);
  });
}

Future<Try<vector<string>>> Persistence::listAllDocumentsForCentroid(
    string centroidId) {
  return threadPool_->addFuture([this, centroidId]() {
    return syncHandle_->listAllDocumentsForCentroid(centroidId);
  });
}

Future<Optional<vector<string>>> Persistence::listAllDocumentsForCentroidOption(
    string centroidId) {
  return threadPool_->addFuture([this, centroidId]() {
    return syncHandle_->listAllDocumentsForCentroidOption(centroidId);
  });
}

Future<Try<vector<string>>> Persistence::listCentroidDocumentRangeFromOffset(
    string centroidId, size_t offset, size_t limit) {
  return threadPool_->addFuture([this, centroidId, offset, limit]() {
    return syncHandle_->listCentroidDocumentRangeFromOffset(
      centroidId, offset, limit
    );
  });
}

Future<Optional<vector<string>>>
Persistence::listCentroidDocumentRangeFromOffsetOption(
    string centroidId, size_t offset, size_t limit) {
  return threadPool_->addFuture([this, centroidId, offset, limit]() {
    return syncHandle_->listCentroidDocumentRangeFromOffsetOption(
      centroidId, offset, limit
    );
  });
}

Future<Try<vector<string>>>
Persistence::listCentroidDocumentRangeFromDocumentId(
    string centroidId, string documentId, size_t limit) {
  return threadPool_->addFuture([this, centroidId, documentId, limit]() {
    return syncHandle_->listCentroidDocumentRangeFromDocumentId(
      centroidId, documentId, limit
    );
  });
}

Future<Optional<vector<string>>>
Persistence::listCentroidDocumentRangeFromDocumentIdOption(
    string centroidId, string documentId, size_t limit) {
  return threadPool_->addFuture([this, centroidId, documentId, limit]() {
    return syncHandle_->listCentroidDocumentRangeFromDocumentIdOption(
      centroidId, documentId, limit
    );
  });
}

Future<Optional<string>> Persistence::getCentroidMetadata(
    string id, string metaName) {
  return threadPool_->addFuture([this, id, metaName]() {
    return syncHandle_->getCentroidMetadata(id, metaName);
  });
}

Future<Try<bool>> Persistence::setCentroidMetadata(
    string id, string metaName, string value) {
  return threadPool_->addFuture([this, id, metaName, value]() {
    return syncHandle_->setCentroidMetadata(id, metaName, value);
  });
}

Future<folly::Unit> Persistence::debugEraseAllData() {
  return threadPool_->addFuture([this](){
    syncHandle_->debugEraseAllData();
  });
}

} // persistence
} // relevanced
