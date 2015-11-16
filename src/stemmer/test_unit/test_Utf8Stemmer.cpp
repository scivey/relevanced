#include <string>
#include <tuple>
#include <vector>
#include "gtest/gtest.h"
#include "stemmer/Utf8Stemmer.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
using namespace std;
using namespace relevanced::stemmer;
using relevanced::thrift_protocol::Language;

TEST(TestUtf8Stemmer, SimpleTest) {
  string text = "embarassingly";
  Utf8Stemmer stemmer(Language::EN);
  size_t offset = stemmer.getStemPos(text.c_str(), text.size());
  string stemmed = text;
  stemmed.erase(offset);
  EXPECT_EQ("embarass", stemmed);
}
