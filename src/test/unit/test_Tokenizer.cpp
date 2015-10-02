#include "gtest/gtest.h"
#include "tokenizer/Tokenizer.h"

using namespace std;
using relevanced::tokenizer::Tokenizer;

TEST(TestTokenizer, SimpleTest) {
    Tokenizer tokenizer;
    auto result = tokenizer.tokenize("this is some text");
    EXPECT_EQ(4, result.size());
    EXPECT_EQ("some", result.at(2));
}
