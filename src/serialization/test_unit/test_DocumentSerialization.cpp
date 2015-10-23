#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <map>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "serialization/serializers.h"
#include "models/ProcessedDocument.h"
#include "text_util/ScoredWord.h"


using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::text_util;
TEST(TestDocumentSerialization, TestBinarySerialization) {
  vector<ScoredWord> scores {
    ScoredWord("foo", 3, 1.82),
    ScoredWord("bar", 3, 9.78)
  };
  ProcessedDocument doc(
      "doc-id", scores, 15.3
  );
  string data;
  serialization::binarySerialize(data, doc);
  EXPECT_TRUE(data != "");
  ProcessedDocument result("");
  serialization::binaryDeserialize(data, &result);
  EXPECT_EQ("doc-id", result.id);
  EXPECT_EQ(2, result.scoredWords.size());
  EXPECT_EQ(15.3, result.magnitude);
}
