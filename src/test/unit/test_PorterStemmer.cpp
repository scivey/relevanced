#include "gtest/gtest.h"
#include <vector>
#include <string>

#include "stemmer/PorterStemmer.h"

using namespace std;
using relevanced::stemmer::PorterStemmer;

TEST(PorterStemmer, TestStem) {
    PorterStemmer stemmer;
    string toStem = "brutalization";
    string stemmed = stemmer.stem(toStem);
    EXPECT_EQ("brutal", stemmed);
}

TEST(PorterStemmer, stemInPlace) {
    PorterStemmer stemmer;
    string toStem = "meeting";
    stemmer.stemInPlace(toStem);
    EXPECT_EQ("meet", toStem);
}
