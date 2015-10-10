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
#include "util/util.h"
#include "persistence/SyncPersistence.h"
#include "models/WordVector.h"
#include "models/Centroid.h"
#include "models/ProcessedDocument.h"


using namespace std;
using namespace folly;

namespace relevanced {
namespace persistence {

using models::WordVector;
using models::ProcessedDocument;
using models::Centroid;

Persistence::Persistence(
  util::UniquePointer<SyncPersistenceIf> syncHandle,
  shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
): syncHandle_(std::move(syncHandle)), threadPool_(threadPool) {}

Future<bool> Persistence::doesDocumentExist(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->doesDocumentExist(id);
  });
}

Future<Try<bool>> Persistence::saveDocument(shared_ptr<ProcessedDocument> doc) {
  return threadPool_->addFuture([this, doc](){
    return syncHandle_->saveDocument(doc);
  });
}

Future<Try<bool>> Persistence::deleteDocument(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->deleteDocument(id);
  });
}

Future<vector<string>> Persistence::listAllDocuments() {
  return threadPool_->addFuture([this](){
    return syncHandle_->listAllDocuments();
  });
}

Future<vector<string>> Persistence::listDocumentRangeFromId(const string &documentId, size_t count) {
  return threadPool_->addFuture([this, documentId, count](){
    return syncHandle_->listDocumentRangeFromId(documentId, count);
  });
}

Future<vector<string>> Persistence::listDocumentRangeFromOffset(size_t offset, size_t count) {
  return threadPool_->addFuture([this, offset, count](){
    return syncHandle_->listDocumentRangeFromOffset(offset, count);
  });
}

Future<Try<shared_ptr<ProcessedDocument>>> Persistence::loadDocument(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->loadDocument(id);
  });
}

Future<Optional<shared_ptr<ProcessedDocument>>> Persistence::loadDocumentOption(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->loadDocumentOption(id);
  });
}

Future<bool> Persistence::doesCentroidExist(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->doesCentroidExist(id);
  });
}

Future<Try<bool>> Persistence::createNewCentroid(const string &id) {
  LOG(INFO) << format("createNewCentroid: {}", id);
  return threadPool_->addFuture([this, id](){
    return syncHandle_->createNewCentroid(id);
  });
}

Future<Try<bool>> Persistence::deleteCentroid(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->deleteCentroid(id);
  });
}

Future<Try<bool>> Persistence::saveCentroid(const string &id, shared_ptr<Centroid> centroid) {
  return threadPool_->addFuture([this, id, centroid](){
    return syncHandle_->saveCentroid(id, centroid);
  });
}

Future<Try<shared_ptr<Centroid>>> Persistence::loadCentroid(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->loadCentroid(id);
  });
}

Future<Optional<shared_ptr<Centroid>>> Persistence::loadCentroidOption(const string &id) {
  return threadPool_->addFuture([this, id](){
    return syncHandle_->loadCentroidOption(id);
  });
}

Future<vector<string>> Persistence::listAllCentroids() {
  return threadPool_->addFuture([this](){
    return syncHandle_->listAllCentroids();
  });
}

Future<vector<string>> Persistence::listCentroidRangeFromOffset(size_t offset, size_t limit) {
  return threadPool_->addFuture([this, offset, limit](){
    return syncHandle_->listCentroidRangeFromOffset(offset, limit);
  });
}

Future<vector<string>> Persistence::listCentroidRangeFromId(const string &startingCentroidId, size_t limit) {
  return threadPool_->addFuture([this, startingCentroidId, limit](){
    return syncHandle_->listCentroidRangeFromId(startingCentroidId, limit);
  });
}

Future<Try<bool>> Persistence::addDocumentToCentroid(const string &centroidId, const string &docId) {
  return threadPool_->addFuture([this, centroidId, docId](){
    return syncHandle_->addDocumentToCentroid(centroidId, docId);
  });
}

Future<Try<bool>> Persistence::removeDocumentFromCentroid(const string &centroidId, const string &docId) {
  return threadPool_->addFuture([this, centroidId, docId](){
    return syncHandle_->removeDocumentFromCentroid(centroidId, docId);
  });
}

Future<Try<bool>> Persistence::doesCentroidHaveDocument(const string &centroidId, const string &docId) {
  return threadPool_->addFuture([this, centroidId, docId](){
    return syncHandle_->doesCentroidHaveDocument(centroidId, docId);
  });
}

Future<Try<vector<string>>> Persistence::listAllDocumentsForCentroid(const string &centroidId) {
  return threadPool_->addFuture([this, centroidId](){
    return syncHandle_->listAllDocumentsForCentroid(centroidId);
  });
}

Future<Optional<vector<string>>> Persistence::listAllDocumentsForCentroidOption(const string &centroidId) {
  return threadPool_->addFuture([this, centroidId](){
    return syncHandle_->listAllDocumentsForCentroidOption(centroidId);
  });
}

Future<Try<vector<string>>> Persistence::listCentroidDocumentRangeFromOffset(const string &centroidId, size_t offset, size_t limit) {
  return threadPool_->addFuture([this, centroidId, offset, limit](){
    return syncHandle_->listCentroidDocumentRangeFromOffset(centroidId, offset, limit);
  });
}

Future<Optional<vector<string>>> Persistence::listCentroidDocumentRangeFromOffsetOption(const string &centroidId, size_t offset, size_t limit) {
  return threadPool_->addFuture([this, centroidId, offset, limit](){
    return syncHandle_->listCentroidDocumentRangeFromOffsetOption(centroidId, offset, limit);
  });
}

Future<Try<vector<string>>> Persistence::listCentroidDocumentRangeFromDocumentId(const string &centroidId, const string &documentId, size_t limit) {
  return threadPool_->addFuture([this, centroidId, documentId, limit](){
    return syncHandle_->listCentroidDocumentRangeFromDocumentId(centroidId, documentId, limit);
  });
}

Future<Optional<vector<string>>> Persistence::listCentroidDocumentRangeFromDocumentIdOption(const string &centroidId, const string &documentId, size_t limit) {
  return threadPool_->addFuture([this, centroidId, documentId, limit](){
    return syncHandle_->listCentroidDocumentRangeFromDocumentIdOption(centroidId, documentId, limit);
  });
}

Future<Optional<string>> Persistence::getCentroidMetadata(const string &centroidId, const string &metadataName) {
  return threadPool_->addFuture([this, centroidId, metadataName](){
    return syncHandle_->getCentroidMetadata(centroidId, metadataName);
  });
}

Future<Try<bool>> Persistence::setCentroidMetadata(const string &centroidId, const string &metadataName, string value) {
  return threadPool_->addFuture([this, centroidId, metadataName, value](){
    return syncHandle_->setCentroidMetadata(centroidId, metadataName, value);
  });
}

} // persistence
} // relevanced

