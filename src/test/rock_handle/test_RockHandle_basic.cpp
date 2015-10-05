#include "gtest/gtest.h"
#include "persistence/RockHandle.h"
#include <vector>
#include <functional>
#include <map>
#include <string>
#include <atomic>
#include <folly/Format.h>

using namespace std;
using namespace folly;
using namespace relevanced::persistence;

string getDataDir() {
  static atomic<int> dirNum {0};
  dirNum++;
  int x = dirNum;
  string dirName = sformat("./test_data/data{}", x);
  return dirName;
}

TEST(TestRockHandle, TestSetGet) {
  RockHandle handle(getDataDir());
  handle.put("x", "x-val");
  handle.put("y", "y-val");
  EXPECT_EQ("x-val", handle.get("x"));
  EXPECT_EQ("y-val", handle.get("y"));
}

TEST(TestRockHandle, TestExists1) {
  RockHandle handle(getDataDir());
  EXPECT_FALSE(handle.exists("x"));
  EXPECT_FALSE(handle.exists("y"));
  handle.put("x", "x-val");
  EXPECT_TRUE(handle.exists("x"));
  EXPECT_FALSE(handle.exists("y"));
  handle.put("y", "y-val");
  EXPECT_TRUE(handle.exists("x"));
  EXPECT_TRUE(handle.exists("y"));
}

TEST(TestRockHandle, TestExists2) {
  RockHandle handle(getDataDir());
  EXPECT_FALSE(handle.exists("x"));
  EXPECT_FALSE(handle.exists("y"));
  handle.put("x", "x-val");
  handle.put("y", "y-val");
  EXPECT_TRUE(handle.exists("x"));
  EXPECT_TRUE(handle.exists("y"));
  handle.del("x");
  EXPECT_FALSE(handle.exists("x"));
  EXPECT_TRUE(handle.exists("y"));
  handle.del("y");
  EXPECT_FALSE(handle.exists("x"));
  EXPECT_FALSE(handle.exists("y"));
}
