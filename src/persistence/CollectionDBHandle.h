#pragma once
#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>
#include "PrefixedRockHandle.h"
#include "RockHandle.h"

#include "util.h"

namespace persistence {

class CollectionDBHandleIf {
public:
  virtual bool doesCollectionExist(const std::string&) = 0;
  virtual bool createCollection(const std::string&) = 0;
  virtual bool doesCollectionHaveDoc(const std::string&, const std::string&) = 0;
  virtual bool addPositiveDocToCollection(const std::string &collectionId, const std::string &docId) = 0;
  virtual bool addNegativeDocToCollection(const std::string &collectionId, const std::string &docId) = 0;
  virtual bool removeDocFromCollection(const std::string &collectionId, const std::string &docId) = 0;
  virtual bool deleteCollection(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listCollections() = 0;
  virtual int getCollectionDocCount(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listCollectionDocs(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listPositiveCollectionDocs(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listNegativeCollectionDocs(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listKnownDocuments() = 0;
  virtual ~CollectionDBHandleIf() = default;
};

class CollectionDBHandle: public CollectionDBHandleIf {
protected:
  util::UniquePointer<RockHandleIf> collectionDocsHandle_;
  util::UniquePointer<RockHandleIf> collectionListHandle_;
  bool addDocToCollection(const std::string&, const std::string&, bool isPositive);
public:
  CollectionDBHandle(util::UniquePointer<RockHandleIf>, util::UniquePointer<RockHandleIf>);
  bool doesCollectionExist(const std::string&) override;
  bool createCollection(const std::string&) override;
  bool doesCollectionHaveDoc(const std::string&, const std::string&) override;
  bool addPositiveDocToCollection(const std::string &collectionId, const std::string &docId) override;
  bool addNegativeDocToCollection(const std::string &collectionId, const std::string &docId) override;
  bool removeDocFromCollection(const std::string &collectionId, const std::string &docId) override;
  bool deleteCollection(const std::string &collectionId) override;
  std::vector<std::string> listCollections() override;
  int getCollectionDocCount(const std::string &collectionId) override;
  std::vector<std::string> listCollectionDocs(const std::string &collectionId) override;
  std::vector<std::string> listPositiveCollectionDocs(const std::string &collectionId) override;
  std::vector<std::string> listNegativeCollectionDocs(const std::string &collectionId) override;
  std::vector<std::string> listKnownDocuments() override;
};

} // persistence

