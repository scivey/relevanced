#include "gtest/gtest.h"
#include "stopwords/StopwordFilter.h"

using namespace std;
using relevanced::stopwords::StopwordFilter;

TEST(TestStopwordFilter, Punctuation) {
  StopwordFilter filter;
  EXPECT_TRUE(filter.isStopword("."));
  EXPECT_TRUE(filter.isStopword(","));
  EXPECT_TRUE(filter.isStopword("\""));
  EXPECT_TRUE(filter.isStopword("'"));
}

TEST(TestStopwordFilter, EnglishSmattering) {
  StopwordFilter filter;
  EXPECT_TRUE(filter.isStopword("the"));
  EXPECT_TRUE(filter.isStopword("a"));
  EXPECT_TRUE(filter.isStopword("an"));
}

TEST(TestStopwordFilter, TooShort) {
  StopwordFilter filter;
  EXPECT_TRUE(filter.isStopword("zz"));
}

TEST(TestStopwordFilter, NonStopwords) {
  StopwordFilter filter;
  EXPECT_FALSE(filter.isStopword("elephant"));
  EXPECT_FALSE(filter.isStopword("amusingly"));
}
