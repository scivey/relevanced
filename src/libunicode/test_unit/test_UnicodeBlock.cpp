#include "gtest/gtest.h"
#include "libunicode/UnicodeBlock.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::libunicode;
using namespace relevanced::util;

TEST(TestGetUnicodeBlock, BasicLatin) {
  uint32_t codepoint = 65; // 'A'
  EXPECT_EQ(
    UnicodeBlock::BASIC_LATIN,
    getUnicodeBlock(codepoint)
  );
}

TEST(TestGetUnicodeBlock, GeneralPunctuation) {
  uint32_t codepoint = 8212; // unicode long dash
  EXPECT_EQ(
    UnicodeBlock::GENERAL_PUNCTUATION,
    getUnicodeBlock(codepoint)
  );
}

TEST(TestGetUnicodeBlock, Latin1Supplement) {
  uint32_t codepoint = 250; // u with accent
  EXPECT_EQ(
    UnicodeBlock::LATIN_1_SUPPLEMENT,
    getUnicodeBlock(codepoint)
  );
}

TEST(TestGetUnicodeBlock, LatinExtendedA) {
  uint32_t codepoint = 312; // an adorably small capital letter K
  EXPECT_EQ(
    UnicodeBlock::LATIN_EXTENDED_A,
    getUnicodeBlock(codepoint)
  );
}