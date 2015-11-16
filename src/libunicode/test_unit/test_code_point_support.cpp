#include "gtest/gtest.h"
#include "libunicode/code_point_support.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::libunicode;
using namespace relevanced::util;

TEST(TestCodePointSupport, Simple) {
  uint32_t codepoint = (uint32_t) ((unsigned char) 'a');
  EXPECT_TRUE(isLetterPoint(codepoint));

  codepoint = (uint32_t) ((unsigned char) '.');
  EXPECT_FALSE(isLetterPoint(codepoint));

}
