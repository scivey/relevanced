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
  static atomic<int> dirNum{0};
  dirNum++;
  int x = dirNum;
  string dirName = sformat("./test_data/data{}", x);
  return dirName;
}

TEST(TestRockHandle, TestPutGet) {
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

TEST(TestRockHandle, TestIterPrefix1) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  vector<string> iterated;
  handle.iterPrefix("x",
                    [&iterated](const string &key, function<void(string &) >,
                                function<void()>) { iterated.push_back(key); });
  vector<string> expected{"x:key1", "x:key2", "x:key3"};
  EXPECT_EQ(expected, iterated);
}

TEST(TestRockHandle, TestIterPrefix2) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  vector<string> keys;
  vector<string> values;
  handle.iterPrefix("x", [&keys, &values](const string &key,
                                          function<void(string &) > read,
                                          function<void()>) {
    keys.push_back(key);
    string val;
    read(val);
    values.push_back(val);
  });
  vector<string> expectedKeys{"x:key1", "x:key2", "x:key3"};
  vector<string> expectedVals{"val1", "val2", "val3"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixEscape) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  vector<string> keys;
  vector<string> values;
  size_t seen = 0;
  handle.iterPrefix("x", [&keys, &values, &seen](const string &key,
                                                 function<void(string &) > read,
                                                 function<void()> escape) {
    seen++;
    if (seen > 4) {
      escape();
      return;
    }
    keys.push_back(key);
    string val;
    read(val);
    values.push_back(val);
  });
  vector<string> expectedKeys{"x:key1", "x:key2", "x:key3", "x:key4"};
  vector<string> expectedVals{"val1", "val2", "val3", "val4"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromOffset1) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromOffset("x", 2, 5,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()>) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key3", "x:key4", "x:key5", "x:key6", "x:key7"};
  vector<string> expectedVals{"val3", "val4", "val5", "val6", "val7"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromOffsetZero) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromOffset("x", 0, 5,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()>) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key1", "x:key2", "x:key3", "x:key4", "x:key5"};
  vector<string> expectedVals{"val1", "val2", "val3", "val4", "val5"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromOffsetNotEnough) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromOffset("x", 2, 5,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()>) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key3", "x:key4", "x:key5"};
  vector<string> expectedVals{"val3", "val4", "val5"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromOffsetTooFar) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");

  vector<string> keys;
  vector<string> values;
  bool iterated = false;
  handle.iterPrefixFromOffset(
      "x", 10, 5, [&iterated, &keys, &values](const string &key,
                                              function<void(string &) > read,
                                              function<void()>) {
        iterated = true;
        keys.push_back(key);
        string val;
        read(val);
        values.push_back(val);
      });
  vector<string> expectedKeys{};
  vector<string> expectedVals{};
  EXPECT_FALSE(iterated);
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}


TEST(TestRockHandle, TestIterPrefixFromOffsetNone) {
  RockHandle handle(getDataDir());
  vector<string> keys;
  vector<string> values;
  bool iterated = false;
  handle.iterPrefixFromOffset(
      "x", 2, 5, [&iterated, &keys, &values](const string &key,
                                             function<void(string &) > read,
                                             function<void()>) {
        iterated = true;
        keys.push_back(key);
        string val;
        read(val);
        values.push_back(val);
      });
  vector<string> expectedKeys{};
  vector<string> expectedVals{};
  EXPECT_FALSE(iterated);
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}


TEST(TestRockHandle, TestIterPrefixFromOffsetEscape) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  size_t counter = 0;
  handle.iterPrefixFromOffset(
      "x", 2, 7, [&keys, &values, &counter](const string &key,
                                            function<void(string &) > read,
                                            function<void()> escape) {
        counter++;
        if (counter > 3) {
          escape();
          return;
        }
        keys.push_back(key);
        string val;
        read(val);
        values.push_back(val);
      });
  vector<string> expectedKeys{"x:key3", "x:key4", "x:key5"};
  vector<string> expectedVals{"val3", "val4", "val5"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}


TEST(TestRockHandle, TestIterPrefixFromOffsetZeroEscape) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  size_t counter = 0;
  handle.iterPrefixFromOffset(
      "x", 0, 6, [&keys, &values, &counter](const string &key,
                                            function<void(string &) > read,
                                            function<void()> escape) {
        counter++;
        if (counter > 3) {
          escape();
          return;
        }
        keys.push_back(key);
        string val;
        read(val);
        values.push_back(val);
      });
  vector<string> expectedKeys{"x:key1", "x:key2", "x:key3"};
  vector<string> expectedVals{"val1", "val2", "val3"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromMember1) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromMember("x", "key1", 4,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()> escape) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key1", "x:key2", "x:key3", "x:key4"};
  vector<string> expectedVals{"val1", "val2", "val3", "val4"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromMember2) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromMember("x", "key3", 4,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()> escape) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key3", "x:key4", "x:key5", "x:key6"};
  vector<string> expectedVals{"val3", "val4", "val5", "val6"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromMemberEmpty) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  bool iterated = false;
  handle.iterPrefixFromMember(
      "y", "key3", 4,
      [&iterated](const string &key, function<void(string &) > read,
                  function<void()> escape) { iterated = true; });
  EXPECT_FALSE(iterated);
}

TEST(TestRockHandle, TestIterPrefixFromMemberTooMany) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key3", "val3");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromMember("x", "key7", 10,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()> escape) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key7", "x:key8", "x:key9"};
  vector<string> expectedVals{"val7", "val8", "val9"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestIterPrefixFromMemberMissing) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  handle.put("x:key2", "val2");
  handle.put("x:key4", "val4");
  handle.put("x:key5", "val5");
  handle.put("x:key6", "val6");
  handle.put("x:key7", "val7");
  handle.put("x:key8", "val8");
  handle.put("x:key9", "val9");

  vector<string> keys;
  vector<string> values;
  handle.iterPrefixFromMember("x", "key3", 3,
                              [&keys, &values](const string &key,
                                               function<void(string &) > read,
                                               function<void()> escape) {
                                keys.push_back(key);
                                string val;
                                read(val);
                                values.push_back(val);
                              });
  vector<string> expectedKeys{"x:key4", "x:key5", "x:key6"};
  vector<string> expectedVals{"val4", "val5", "val6"};
  EXPECT_EQ(expectedVals, values);
  EXPECT_EQ(expectedKeys, keys);
}

TEST(TestRockHandle, TestEraseEverythingSimple) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  string val;
  EXPECT_TRUE(handle.get("x:key1", val));
  EXPECT_EQ("val1", val);
  handle.eraseEverything();
  val = "";
  EXPECT_FALSE(handle.get("x:key1", val));
  EXPECT_EQ("", val);
}

TEST(TestRockHandle, TestEraseEverythingStillGood) {
  RockHandle handle(getDataDir());
  handle.put("x:key1", "val1");
  string val;
  EXPECT_TRUE(handle.get("x:key1", val));
  EXPECT_EQ("val1", val);
  handle.eraseEverything();
  val = "";
  EXPECT_FALSE(handle.get("x:key1", val));
  EXPECT_EQ("", val);
  handle.put("x:key1", "val1-again");
  EXPECT_TRUE(handle.get("x:key1", val));
  EXPECT_EQ("val1-again", val);
  handle.put("x:key2", "val2");
  EXPECT_TRUE(handle.get("x:key2", val));
  EXPECT_EQ("val2", val);
}
