#include <vector>
#include <folly/Synchronized.h>

#include "gtest/gtest.h"
#include "text_util/ScoredWord.h"
#include "util/util.h"
#include "util/ConcurrentMap.h"


using namespace std;
using namespace folly;

using namespace relevanced;
using namespace relevanced::text_util;
using namespace relevanced::util;

class Something {
  static Synchronized<vector<string>>& getIdVector() {
    static Synchronized<vector<string>> idVector;
    return idVector;
  }
  static void onDelete(string instanceId) {
    getIdVector()->push_back(instanceId);
  }
public:
  static void resetDeletedIds() {
    getIdVector()->clear();
  }
  static vector<string> getDeletedIds() {
    vector<string> ids;
    auto deleted = getIdVector();
    SYNCHRONIZED(deleted) {
      for (auto &id: deleted) {
        ids.push_back(id);
      }
    }
    return ids;
  }
  string id {"DEFAULT"};
  Something(){}
  Something(string id): id(id) {}
  ~Something(){
    Something::onDelete(id);
  }
};

TEST(TestConcurrentMap, Simple) {
  {
    ConcurrentMap<string, Something> aMap {10};
    aMap.insertOrUpdate("x", UniquePointer<Something>(new Something("x")));
    aMap.insertOrUpdate("y", UniquePointer<Something>(new Something("y")));
  }
  auto deleted = setOfVec(Something::getDeletedIds());
  set<string> expected {"x", "y"};
  EXPECT_EQ(expected, deleted);
  Something::resetDeletedIds();
}


