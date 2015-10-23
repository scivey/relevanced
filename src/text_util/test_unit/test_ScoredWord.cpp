#include "gtest/gtest.h"
#include "text_util/ScoredWord.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::text_util;
using namespace relevanced::util;

TEST(TestScoredWord, Simple) {
  string dog {"dog"};
  ScoredWord word(dog.c_str(), 3, 1.5);
  string copied = word.word;
  EXPECT_EQ("dog", copied);
  EXPECT_EQ(1.5, word.score);
  EXPECT_EQ(3, word.getSize());
}

TEST(TestScoredWord, WordTooLong) {
  string fish {"fish|fish|fish|fish|fish|"};
  ScoredWord word(fish.c_str(), fish.size(), 1.5);
  string copied = word.word;
  EXPECT_EQ("fish|fish|fish|fish|fi", copied);
  EXPECT_EQ(1.5, word.score);
  EXPECT_EQ(23, word.getSize());
}