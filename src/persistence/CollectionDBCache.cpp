#include <memory>
#include <set>
#include <vector>
#include <string>

#include <folly/Synchronized.h>

#include "CollectionDBCache.h"

using namespace folly;
using namespace std;

namespace persistence {

bool CollectionDBCache::createCollection(const string &id) {
  auto positiveDocs = std::make_shared<set<string>>();
  auto negativeDocs = std::make_shared<set<string>>();
  SYNCHRONIZED_DUAL(positive, positiveDocSets_, negative, negativeDocSets_) {
    positive.insert(make_pair(id, std::move(positiveDocs)));
    negative.insert(make_pair(id, std::move(negativeDocs)));
  }
  return true;
}

bool CollectionDBCache::deleteCollection(const string &id) {
  SYNCHRONIZED_DUAL(positive, positiveDocSets_, negative, negativeDocSets_) {
    if (positive.find(id) == positive.end()) {
      return false;
    }
    positive.erase(id);
    negative.erase(id);
    return true;
  }
  return true;
}


bool CollectionDBCache::removeDocFromCollection(const string &collId, const string &docId) {
  SYNCHRONIZED(positiveDocSets_) {
    if (positiveDocSets_.find(collId) != positiveDocSets_.end()) {
      positiveDocSets_[collId]->erase(docId);
    }
  }
  SYNCHRONIZED(negativeDocSets_) {
    if (negativeDocSets_.find(collId) != negativeDocSets_.end()) {
      negativeDocSets_[collId]->erase(docId);
    }
  }
  return true;
}


bool CollectionDBCache::addPositiveDocToCollection(const string &collId, const string &docId) {
  SYNCHRONIZED(positiveDocSets_) {
    if (positiveDocSets_.find(collId) == positiveDocSets_.end()) {
      return false;
    }
    positiveDocSets_[collId]->insert(docId);
    return true;
  }
}

bool CollectionDBCache::addNegativeDocToCollection(const string &collId, const string &docId) {
  SYNCHRONIZED(negativeDocSets_) {
    if (negativeDocSets_.find(collId) == negativeDocSets_.end()) {
      return false;
    }
    negativeDocSets_[collId]->insert(docId);
    return true;
  }
}

vector<string> CollectionDBCache::listCollectionDocs(const string &collId) {
  vector<string> result;
  SYNCHRONIZED_DUAL(positive, positiveDocSets_, negative, negativeDocSets_) {
    for (auto &elem: positive) {
      result.push_back(elem.first);
    }
    for (auto &elem: negative) {
      result.push_back(elem.first);
    }
  }
  return result;
}

vector<string> CollectionDBCache::listPositiveCollectionDocs(const string &collId) {
  vector<string> result;
  SYNCHRONIZED(positiveDocSets_) {
    if (positiveDocSets_.find(collId) == positiveDocSets_.end()) {
      return result;
    }
    for (auto &elem: *positiveDocSets_[collId]) {
      result.push_back(elem);
    }
  }
  return result;
}

vector<string> CollectionDBCache::listNegativeCollectionDocs(const string &collId) {
  vector<string> result;
  SYNCHRONIZED(negativeDocSets_) {
    if (negativeDocSets_.find(collId) == negativeDocSets_.end()) {
      return result;
    }
    for (auto &elem: *negativeDocSets_[collId]) {
      result.push_back(elem);
    }
  }
  return result;
}

vector<string> CollectionDBCache::listCollections() {
  vector<string> result;
  SYNCHRONIZED(positiveDocSets_) {
    for (auto &elem: positiveDocSets_) {
      result.push_back(elem.first);
    }
  }
  return result;
}

bool CollectionDBCache::doesCollectionHaveDoc(const string &collId, const string &docId) {
  SYNCHRONIZED(positiveDocSets_) {
    if (positiveDocSets_.find(collId) == positiveDocSets_.end()) {
      return false;
    }
    auto docs = positiveDocSets_[collId];
    if (docs->find(docId) != docs->end()) {
      return true;
    }
  }
  SYNCHRONIZED(negativeDocSets_) {
    if (negativeDocSets_.find(collId) == negativeDocSets_.end()) {
      return false;
    }
    auto docs = negativeDocSets_[collId];
    if (docs->find(docId) != docs->end()) {
      return true;
    }
  }
  return false;
}

bool CollectionDBCache::doesCollectionExist(const string &id) {
  SYNCHRONIZED(positiveDocSets_) {
    if (positiveDocSets_.find(id) == positiveDocSets_.end()) {
      return false;
    }
    return true;
  }
}

int CollectionDBCache::getCollectionDocCount(const string &id) {
  auto docs = listCollectionDocs(id);
  return docs.size();
}

} // persistence
