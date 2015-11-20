#include "gtest/gtest.h"
#include "libunicode/code_point_support.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::libunicode;
using namespace relevanced::util;

TEST(TestIsLetterPoint, Simple) {
  uint32_t codePoint = (uint32_t) ((unsigned char) 'a');
  EXPECT_TRUE(isLetterPoint(codePoint));

  codePoint = (uint32_t) ((unsigned char) '.');
  EXPECT_FALSE(isLetterPoint(codePoint));
}

TEST(TestIsLetterPoint, UnicodePunctuation) {
  uint32_t codePoint = 8212; // unicode long dash
  EXPECT_FALSE(isLetterPoint(codePoint));
}

TEST(TestIsLetterPoint, Numbers) {
  for (unsigned char c = 0; c < 10; c++) {
    uint32_t codePoint = c;
    EXPECT_FALSE(isLetterPoint(codePoint));
  }
}

TEST(TestIsLetterPoint, GermanLetter) {
  uint32_t codePoint = 223; // Eszett, the big funny B
  EXPECT_TRUE(isLetterPoint(codePoint));
}

TEST(TestIsLetterPoint, SpanishLetter) {
  uint32_t codePoint = 209; // capital ene (n with tilde)
  EXPECT_TRUE(isLetterPoint(codePoint));
}
