#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <unordered_map>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "serialization/serializers.h"
#include "models/Centroid.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::text_util;
TEST(TestCentroidSerialization, TestBinarySerialization) {
  Centroid centroid {
    "centroid-id",
    unordered_map<string, double> {{"fish", 0.4}, {"cats", 0.5}, {"dogs", 1.6}},
    22.8
  };
  string data;
  serialization::binarySerialize(data, centroid);
  EXPECT_TRUE(data != "");

  Centroid result;
  serialization::binaryDeserialize(data, &result);

  EXPECT_EQ("centroid-id", result.id);
  unordered_map<string, double> expectedScores {
    {"fish", 0.4},
    {"cats", 0.5},
    {"dogs", 1.6}
  };
  EXPECT_EQ(expectedScores, result.wordVector.scores);
  EXPECT_EQ(22.8, result.wordVector.magnitude);
}
