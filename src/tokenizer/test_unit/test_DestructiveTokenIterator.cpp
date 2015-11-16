#include <string>
#include <tuple>
#include <vector>
#include "gtest/gtest.h"
#include "tokenizer/DestructiveTokenIterator.h"

using namespace std;
using relevanced::tokenizer::DestructiveTokenIterator;

TEST(TestDestructiveTokenIterator, SimpleTest) {
  string text = "this is some text";
  DestructiveTokenIterator iter(text);
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
    "this", "some", "text"
  };
  EXPECT_EQ(expected, tokens);
}

TEST(TestDestructiveTokenIterator, TestUpperCase) {
  string text = "this is some TEXt";
  DestructiveTokenIterator iter(text);
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
    "this", "some", "text"
  };
  EXPECT_EQ(expected, tokens);
}


TEST(TestDestructiveTokenIterator, TestRepeatedPunctuation) {
  string text = "this is some\"\"TEXt";
  DestructiveTokenIterator iter(text);
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
    "this", "some", "text"
  };
  EXPECT_EQ(expected, tokens);
}
