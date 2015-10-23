#include <string>
#include <tuple>
#include <vector>
#include "gtest/gtest.h"
#include "stemmer/Utf8Stemmer.h"

using namespace std;
using namespace relevanced::stemmer;

TEST(TestUtf8Stemmer, SimpleTest) {
  string text = "embarassingly";
  Utf8Stemmer stemmer;
  size_t offset = stemmer.getStemPos(text.c_str(), text.size());
  string stemmed = text;
  stemmed.erase(offset);
  EXPECT_EQ("embarass", stemmed);
}
