#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

#include "gtest/gtest.h"
#include "models/WordVector.h"
#include "models/ProcessedDocument.h"
#include "text_util/ScoredWord.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::text_util;


TEST(TestWordVector, AgainstOtherWordVector) {
  WordVector words(
    unordered_map<string, double> {{"fish", 0.3}, {"cat", 0.7}}, 1.0
  );
  WordVector otherWords(
    unordered_map<string, double> {{"something", 0.5}, {"cat", 0.5}}, 1.0
  );
  auto score = words.score(&otherWords);
  EXPECT_TRUE(score < 1);
  EXPECT_TRUE(score > 0.3);
}

TEST(TestWordVector, AgainstProcessedDocument) {
  WordVector words(
    unordered_map<string, double> {{"fish", 0.3}, {"cat", 0.7}}, 1.0
  );
  vector<ScoredWord> docWords {
    ScoredWord("something", 9, 0.5),
    ScoredWord("cat", 3, 0.5)
  };
  ProcessedDocument doc("some-doc", docWords, 1.0);
  auto score = words.score(&doc);
  EXPECT_TRUE(score < 1);
  EXPECT_TRUE(score > 0.3);
}
