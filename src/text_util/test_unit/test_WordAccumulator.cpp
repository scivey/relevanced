#include "gtest/gtest.h"
#include "text_util/WordAccumulator.h"
#include "text_util/StringView.h"
#include "util/util.h"


using namespace std;
using namespace relevanced;
using namespace relevanced::text_util;
using namespace relevanced::util;


TEST(TestWordAccumulator, Simple) {
  string dog {"dog"};
  string cat {"cat"};
  string fish {"fish"};
  vector<StringView> views;
  for (size_t i = 0; i < 5; i++) {
    views.emplace_back(dog.c_str(), dog.size());
    views.emplace_back(cat.c_str(), cat.size());
    views.emplace_back(fish.c_str(), fish.size());
  }
  WordAccumulator accumulator {10};
  for (auto &word: views) {
    accumulator.add(word);
  }
  accumulator.build();
  auto scores = accumulator.getScores();
  set<string> scoreKeys;
  for (auto &score: scores) {
    scoreKeys.insert(score.word);
  }
  set<string> expectedKeys {"dog", "cat", "fish"};
  EXPECT_EQ(expectedKeys, scoreKeys);
}
