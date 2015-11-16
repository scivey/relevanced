#include "gtest/gtest.h"
#include "stopwords/StopwordFilter.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
using namespace std;
using relevanced::stopwords::StopwordFilter;
using relevanced::thrift_protocol::Language;

TEST(TestStopwordFilter, EnglishSmattering) {
  StopwordFilter filter;
  EXPECT_TRUE(filter.isStopword("the", Language::EN));
  EXPECT_TRUE(filter.isStopword("a", Language::EN));
  EXPECT_TRUE(filter.isStopword("an", Language::EN));
}

TEST(TestStopwordFilter, TooShort) {
  StopwordFilter filter;
  EXPECT_TRUE(filter.isStopword("zz", Language::EN));
  EXPECT_TRUE(filter.isStopword("zz", Language::FR));
  EXPECT_TRUE(filter.isStopword("zz", Language::DE));

}

TEST(TestStopwordFilter, NonStopwords) {
  StopwordFilter filter;
  EXPECT_FALSE(filter.isStopword("elephant", Language::EN));
  EXPECT_FALSE(filter.isStopword("amusingly", Language::EN));
}
