#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <map>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "serialization/serializers.h"
#include "models/ProcessedDocument.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;

TEST(TestDocumentSerialization, TestBinarySerialization) {
  ProcessedDocument doc(
      "doc-id", map<string, double>{{"foo", 1.82}, {"bar", 9.78}}, 15.3);
  string data;
  serialization::binarySerialize(data, doc);
  EXPECT_TRUE(data != "");
  ProcessedDocument result("");
  serialization::binaryDeserialize(data, &result);
  EXPECT_EQ("doc-id", result.id);
  auto wordVec = result.wordVector;
  EXPECT_EQ(2, wordVec.scores.size());
  EXPECT_EQ(1.82, wordVec.scores["foo"]);
  EXPECT_EQ(9.78, wordVec.scores["bar"]);
  EXPECT_EQ(15.3, wordVec.magnitude);
}
