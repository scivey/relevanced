#pragma once
#include <folly/Synchronized.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>

namespace persistence {

class CollectionDBCache {
protected:
  folly::Synchronized<
    std::map<
      std::string,
      std::shared_ptr<std::set<std::string>>
    >
  > positiveDocSets_;
  folly::Synchronized<
    std::map<
      std::string,
      std::shared_ptr<std::set<std::string>>
    >
  > negativeDocSets_;
public:
  bool deleteCollection(const std::string &collectionId);
  bool doesCollectionExist(const std::string &collectionId);
  bool removeDocFromCollection(const std::string &collectionId, const std::string &docId);
  bool addPositiveDocToCollection(const std::string &collectionId, const std::string &docId);
  bool addNegativeDocToCollection(const std::string &collectionId, const std::string &docId);
  std::vector<std::string> listCollectionDocs(const std::string &collectionId);
  std::vector<std::string> listPositiveCollectionDocs(const std::string &collectionId);
  std::vector<std::string> listNegativeCollectionDocs(const std::string &collectionId);
  std::vector<std::string> listCollections();
  bool createCollection(const std::string &collectionId);
  bool doesCollectionHaveDoc(const std::string &collectionId, const std::string &docId);
  int getCollectionDocCount(const std::string &collectionId);

};

} // persistence
