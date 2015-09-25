#pragma once
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

namespace persistence {

class DocumentDBCache {
protected:
  folly::Synchronized<
    std::set<std::string>
  > docIds_;
public:
  bool exists(const std::string &docId);
  void add(const std::string &docId);
  void remove(const std::string &docId);
};

} // persistence
