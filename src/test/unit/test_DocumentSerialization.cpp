#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "serialization/serializers.h"
#include "models/ProcessedDocument.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;

TEST(TestDocumentSerialization, TestBinarySerialization) {
    ProcessedDocument doc("doc-id");
    doc.normalizedWordCounts.insert(make_pair("foo", 1.82));
    doc.normalizedWordCounts.insert(make_pair("bar", 9.78));
    string data;
    serialization::binarySerialize(data, doc);
    EXPECT_TRUE(data != "");
    ProcessedDocument result("");
    serialization::binaryDeserialize(data, &result);
    EXPECT_EQ("doc-id", result.id);
    EXPECT_EQ(2, result.normalizedWordCounts.size());
    EXPECT_EQ(1.82, result.normalizedWordCounts["foo"]);
    EXPECT_EQ(9.78, result.normalizedWordCounts["bar"]);
}
