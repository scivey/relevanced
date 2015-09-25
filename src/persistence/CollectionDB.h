#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include "CollectionDBHandle.h"
#include "CollectionDBCache.h"

#include "util.h"

namespace persistence {

class CollectionDBIf {
public:
  virtual folly::Future<bool>
    doesCollectionExist(const std::string&) = 0;
  virtual folly::Future<bool>
    createCollection(const std::string&) = 0;

  virtual folly::Future<bool>
    doesCollectionHaveDoc(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    addPositiveDocToCollection(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    addNegativeDocToCollection(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    removeDocFromCollection(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    deleteCollection(const std::string&) = 0;

  virtual folly::Future<std::vector<std::string>>
    listCollections() = 0;

  virtual folly::Future<int>
    getCollectionDocCount(const std::string &) = 0;

  virtual folly::Future<std::vector<std::string>>
    listCollectionDocs(const string &collectionId) = 0;

  virtual folly::Future<std::vector<std::string>>
    listPositiveCollectionDocs(const string &collectionId) = 0;

  virtual folly::Future<std::vector<std::string>>
    listNegativeCollectionDocs(const string &collectionId) = 0;

  virtual folly::Future<std::vector<std::string>>
    listKnownDocuments() = 0;

  virtual ~CollectionDBIf() = default;
};

class CollectionDB: public CollectionDBIf {
protected:
  util::UniquePointer<CollectionDBHandleIf> dbHandle_;
  std::shared_ptr<CollectionDBCache> dbCache_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
  CollectionDB(CollectionDB const&) = delete;
  void operator=(CollectionDB const&) = delete;
public:
  CollectionDB(
    util::UniquePointer<CollectionDBHandleIf> dbHandle,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
  );
  folly::Future<bool> doesCollectionExist(const std::string &collectionId);
  folly::Future<bool> createCollection(const std::string &collectionId);
  folly::Future<bool> doesCollectionHaveDoc(const std::string &collectionId, const std::string &docId);
  folly::Future<bool> addPositiveDocToCollection(const std::string &collectionId, const std::string &docId);
  folly::Future<bool> addNegativeDocToCollection(const std::string &collectionId, const std::string &docId);
  folly::Future<bool> removeDocFromCollection(const std::string &collectionId, const std::string &docId);
  folly::Future<bool> deleteCollection(const std::string &collectionId);
  folly::Future<std::vector<std::string>> listCollections();
  folly::Future<int> getCollectionDocCount(const std::string &collectionId);
  folly::Future<std::vector<std::string>> listCollectionDocs(const std::string &collectionId);
  folly::Future<std::vector<std::string>> listPositiveCollectionDocs(const std::string &collectionId);
  folly::Future<std::vector<std::string>> listNegativeCollectionDocs(const std::string &collectionId);
  folly::Future<std::vector<std::string>> listKnownDocuments();
};

} // persistence
