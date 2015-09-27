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
  virtual bool doesCollectionHaveDocument(const std::string&, const std::string&) = 0;
  virtual bool addPositiveDocumentToCollection(const std::string &collectionId, const std::string &docId) = 0;
  virtual bool addNegativeDocumentToCollection(const std::string &collectionId, const std::string &docId) = 0;
  virtual bool removeDocumentFromCollection(const std::string &collectionId, const std::string &docId) = 0;
  virtual bool deleteCollection(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listCollections() = 0;
  virtual int getCollectionDocumentCount(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listCollectionDocuments(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listPositiveCollectionDocuments(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listNegativeCollectionDocuments(const std::string &collectionId) = 0;
  virtual std::vector<std::string> listKnownDocuments() = 0;
  virtual ~CollectionDBHandleIf() = default;
};

class CollectionDBHandle: public CollectionDBHandleIf {
protected:
  util::UniquePointer<RockHandleIf> collectionDocsHandle_;
  util::UniquePointer<RockHandleIf> collectionListHandle_;
  bool addDocumentToCollection(const std::string&, const std::string&, bool isPositive);
public:
  CollectionDBHandle(util::UniquePointer<RockHandleIf>, util::UniquePointer<RockHandleIf>);
  bool doesCollectionExist(const std::string&) override;
  bool createCollection(const std::string&) override;
  bool doesCollectionHaveDocument(const std::string&, const std::string&) override;
  bool addPositiveDocumentToCollection(const std::string &collectionId, const std::string &docId) override;
  bool addNegativeDocumentToCollection(const std::string &collectionId, const std::string &docId) override;
  bool removeDocumentFromCollection(const std::string &collectionId, const std::string &docId) override;
  bool deleteCollection(const std::string &collectionId) override;
  std::vector<std::string> listCollections() override;
  int getCollectionDocumentCount(const std::string &collectionId) override;
  std::vector<std::string> listCollectionDocuments(const std::string &collectionId) override;
  std::vector<std::string> listPositiveCollectionDocuments(const std::string &collectionId) override;
  std::vector<std::string> listNegativeCollectionDocuments(const std::string &collectionId) override;
  std::vector<std::string> listKnownDocuments() override;
};

} // persistence

