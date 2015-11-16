#include "gtest/gtest.h"
#include "libunicode/CodePointIterator.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::libunicode;
using namespace relevanced::util;

TEST(TestCodePointIterator, Simple) {
  string text {"this is some text"};
  vector<uint32_t> expected {
    116, 104, 105, 115, 32, 105,
    115, 32, 115, 111, 109, 101,
    32, 116, 101, 120, 116
  };
  vector<uint32_t> result;
  CodePointIterator iter(text);
  for (auto codepoint: iter) {
    result.push_back(codepoint);
  }
  EXPECT_EQ(expected, result);
}
