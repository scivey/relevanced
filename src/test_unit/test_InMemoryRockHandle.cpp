#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include "util.h"
#include "TestHelpers.h"
#include "persistence/InMemoryRockHandle.h"

using namespace std;
using persistence::InMemoryRockHandle;
using util::UniquePointer;
using ::testing::Return;
using ::testing::_;

TEST(InMemoryRockHandle, TestPutGet) {
  InMemoryRockHandle rockHandle("foo");
  string xVal {"x-val"};
  EXPECT_TRUE(rockHandle.put("x-key", xVal));
  EXPECT_EQ("x-val", rockHandle.get("x-key"));
  string gottenInPlace;
  EXPECT_TRUE(rockHandle.get("x-key", gottenInPlace));
  EXPECT_EQ("x-val", gottenInPlace);
}

TEST(InMemoryRockHandle, TestGetMissing) {
  InMemoryRockHandle rockHandle("foo");
  EXPECT_EQ("", rockHandle.get("x-key"));
  string gottenInPlace;
  EXPECT_FALSE(rockHandle.get("x-key", gottenInPlace));
  EXPECT_EQ("", gottenInPlace);
}

TEST(InMemoryRockHandle, TestExistsMissing) {
  InMemoryRockHandle rockHandle("foo");
  EXPECT_FALSE(rockHandle.exists("x-key"));
}

TEST(InMemoryRockHandle, TestExistsForReals1) {
  InMemoryRockHandle rockHandle("foo");
  EXPECT_FALSE(rockHandle.exists("x-key"));
  string xVal {"x-val"};
  EXPECT_TRUE(rockHandle.put("x-key", xVal));
  EXPECT_TRUE(rockHandle.exists("x-key"));
}

TEST(InMemoryRockHandle, TestExistsForReals2) {
  InMemoryRockHandle rockHandle("foo");
  string xVal {"x-val"};
  EXPECT_TRUE(rockHandle.put("x-key", xVal));
  EXPECT_TRUE(rockHandle.exists("x-key"));
}

TEST(InMemoryRockHandle, TestDeleteMissing) {
  InMemoryRockHandle rockHandle("foo");
  EXPECT_FALSE(rockHandle.del("x-key"));
}

TEST(InMemoryRockHandle, TestDeleteHappy) {
  InMemoryRockHandle rockHandle("foo");
  string xVal {"x-val"};
  rockHandle.put("x-key", xVal);
  EXPECT_TRUE(rockHandle.exists("x-key"));
  EXPECT_TRUE(rockHandle.del("x-key"));
  EXPECT_FALSE(rockHandle.exists("x-key"));
  EXPECT_EQ("", rockHandle.get("x-key"));
}

TEST(InMemoryRockHandle, TestCRUD) {
  InMemoryRockHandle rockHandle("foo");
  string xVal {"x-val"};
  string yVal {"y-val"};
  EXPECT_FALSE(rockHandle.exists("x-key"));
  EXPECT_FALSE(rockHandle.exists("y-key"));

  EXPECT_TRUE(rockHandle.put("x-key", xVal));
  EXPECT_TRUE(rockHandle.exists("x-key"));
  EXPECT_FALSE(rockHandle.exists("y-key"));

  EXPECT_TRUE(rockHandle.put("y-key", yVal));
  EXPECT_TRUE(rockHandle.exists("x-key"));
  EXPECT_TRUE(rockHandle.exists("y-key"));

  EXPECT_EQ("x-val", rockHandle.get("x-key"));
  EXPECT_EQ("y-val", rockHandle.get("y-key"));
  EXPECT_TRUE(rockHandle.del("y-key"));

  EXPECT_TRUE(rockHandle.exists("x-key"));
  EXPECT_FALSE(rockHandle.exists("y-key"));
  EXPECT_EQ("x-val", rockHandle.get("x-key"));
  EXPECT_EQ("", rockHandle.get("y-key"));

  EXPECT_FALSE(rockHandle.del("y-key"));

  EXPECT_TRUE(rockHandle.del("x-key"));
  EXPECT_FALSE(rockHandle.exists("x-key"));
  EXPECT_EQ("", rockHandle.get("x-key"));
}

TEST(InMemoryRockHandle, TestIterAllBasic) {
  InMemoryRockHandle rockHandle("foo");
  map<string, string> values {
    {"x-key", "x-val"},
    {"y-key", "y-val"},
    {"z-key", "z-val"}
  };
  for (auto &elem: values) {
    rockHandle.put(elem.first, elem.second);
  }
  vector<string> keysSeen;
  vector<string> valuesSeen;
  rockHandle.iterAll([&keysSeen, &valuesSeen](const string& key, function<void(string&)> read, function<void()> escape) {
    keysSeen.push_back(key);
    string val;
    read(val);
    valuesSeen.push_back(val);
  });
  EXPECT_EQ(3, keysSeen.size());
  EXPECT_EQ(3, valuesSeen.size());
  EXPECT_EQ("x-key", keysSeen.at(0));
  EXPECT_EQ("y-key", keysSeen.at(1));
  EXPECT_EQ("z-key", keysSeen.at(2));
  EXPECT_EQ("x-val", valuesSeen.at(0));
  EXPECT_EQ("y-val", valuesSeen.at(1));
  EXPECT_EQ("z-val", valuesSeen.at(2));
}

TEST(InMemoryRockHandle, TestIterAllEscape) {
  InMemoryRockHandle rockHandle("foo");
  map<string, string> values {
    {"a-key", "a-val"},
    {"b-key", "b-val"},
    {"c-key", "c-val"},
    {"d-key", "d-val"},
    {"e-key", "e-val"},
    {"f-key", "f-val"}
  };
  for (auto &elem: values) {
    rockHandle.put(elem.first, elem.second);
  }
  vector<string> keysSeen;
  vector<string> valuesSeen;
  size_t counter = 0;
  rockHandle.iterAll([&keysSeen, &valuesSeen, &counter](const string& key, function<void(string&)> read, function<void()> escape) {
    counter++;
    if (counter > 4) {
      escape();
    } else {
      keysSeen.push_back(key);
      string val;
      read(val);
      valuesSeen.push_back(val);
    }
  });
  EXPECT_EQ(4, keysSeen.size());
  EXPECT_EQ(4, valuesSeen.size());
  EXPECT_EQ("a-key", keysSeen.at(0));
  EXPECT_EQ("b-key", keysSeen.at(1));
  EXPECT_EQ("c-key", keysSeen.at(2));
  EXPECT_EQ("d-key", keysSeen.at(3));
  EXPECT_EQ("a-val", valuesSeen.at(0));
  EXPECT_EQ("b-val", valuesSeen.at(1));
  EXPECT_EQ("c-val", valuesSeen.at(2));
  EXPECT_EQ("d-val", valuesSeen.at(3));
}

TEST(InMemoryRockHandle, TestIterRange) {
  InMemoryRockHandle rockHandle("foo");
  map<string, string> values {
    {"a-key", "a-val"},
    {"b-key", "b-val"},
    {"c-key", "c-val"},
    {"d-key", "d-val"},
    {"e-key", "e-val"},
    {"f-key", "f-val"}
  };
  for (auto &elem: values) {
    rockHandle.put(elem.first, elem.second);
  }
  vector<string> keysSeen;
  vector<string> valuesSeen;
  rockHandle.iterRange("b", "e", [&keysSeen, &valuesSeen](const string& key, function<void(string&)> read, function<void()> escape) {
    keysSeen.push_back(key);
    string val;
    read(val);
    valuesSeen.push_back(val);
  });
  EXPECT_EQ(3, keysSeen.size());
  EXPECT_EQ(3, valuesSeen.size());
  EXPECT_EQ("b-key", keysSeen.at(0));
  EXPECT_EQ("c-key", keysSeen.at(1));
  EXPECT_EQ("d-key", keysSeen.at(2));
  EXPECT_EQ("b-val", valuesSeen.at(0));
  EXPECT_EQ("c-val", valuesSeen.at(1));
  EXPECT_EQ("d-val", valuesSeen.at(2));
}


TEST(InMemoryRockHandle, TestIterPrefix) {
  InMemoryRockHandle rockHandle("foo");
  map<string, string> values {
    {"bar:x", "bar-x-val"},
    {"bar:y", "bar-y-val"},
    {"foo:x", "foo-x-val"},
    {"foo:y", "foo-y-val"},
    {"zap:x", "zap-x-val"},
    {"zap:y", "zap-y-val"}
  };
  for (auto &elem: values) {
    rockHandle.put(elem.first, elem.second);
  }
  vector<string> keysSeen;
  vector<string> valuesSeen;
  rockHandle.iterPrefix("foo", [&keysSeen, &valuesSeen](const string& key, function<void(string&)> read, function<void()> escape) {
    keysSeen.push_back(key);
    string val;
    read(val);
    valuesSeen.push_back(val);
  });
  EXPECT_EQ(2, keysSeen.size());
  EXPECT_EQ(2, valuesSeen.size());
  EXPECT_EQ("foo:x", keysSeen.at(0));
  EXPECT_EQ("foo:y", keysSeen.at(1));
  EXPECT_EQ("foo-x-val", valuesSeen.at(0));
  EXPECT_EQ("foo-y-val", valuesSeen.at(1));
}


