#include "gtest/gtest.h"
#include "libunicode/UnicodeBlock.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::libunicode;
using namespace relevanced::util;

TEST(TestGetUnicodeBlock, Simple) {
  uint32_t codepoint = 65; // 'A'
  EXPECT_EQ(
    UnicodeBlock::BASIC_LATIN,
    getUnicodeBlock(codepoint)
  );
}
