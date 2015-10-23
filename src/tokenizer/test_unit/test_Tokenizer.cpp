#include <string>
#include <tuple>
#include <vector>
#include "gtest/gtest.h"
#include "tokenizer/DestructiveConllTokenIterator.h"

using namespace std;
using relevanced::tokenizer::DestructiveConllTokenIterator;

TEST(TestTokenizer, SimpleTest) {
  string text = "this is some text";
  DestructiveConllTokenIterator iter(text.data(), text.size());
  vector<string> tokens;
  tuple<bool, size_t, size_t> current;
  while (iter.next(current)) {
    if (!get<0>(current)) {
      break;
    }
    auto start = get<1>(current);
    tokens.push_back(text.substr(start, get<2>(current) - start));
  }
  vector<string> expected {
    "this", "is", "some", "text"
  };
  EXPECT_EQ(expected, tokens);
}

TEST(TestTokenizer, UpperCase) {
  string text = "this is Some tExt";
  DestructiveConllTokenIterator iter(text.data(), text.size());
  vector<string> tokens;
  tuple<bool, size_t, size_t> current;
  while (iter.next(current)) {
    if (!get<0>(current)) {
      break;
    }
    auto start = get<1>(current);
    tokens.push_back(text.substr(start, get<2>(current) - start));
  }
  vector<string> expected {
    "this", "is", "some", "text"
  };
  EXPECT_EQ(expected, tokens);
  EXPECT_EQ("this is some text", text);
}
