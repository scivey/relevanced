#include "DocumentDBCache.h"

#include <memory>
#include <cassert>
#include <vector>
#include <set>
#include <string>
#include <folly/Synchronized.h>

#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>

using namespace std;
using namespace folly;

namespace persistence {

bool DocumentDBCache::exists(const string &id) {
  bool doesExist {false};
  SYNCHRONIZED(docIds_) {
    doesExist = (docIds_.find(id) != docIds_.end());
  }
  return doesExist;
}

void DocumentDBCache::add(const string &id) {
  SYNCHRONIZED(docIds_) {
    docIds_.insert(id);
  }
}

void DocumentDBCache::remove(const string &id) {
  SYNCHRONIZED(docIds_) {
    docIds_.erase(id);
  }
}

} // persistence
