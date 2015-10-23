#include "gtest/gtest.h"
#include "centroid_update_worker/DocumentAccumulator.h"
#include "models/ProcessedDocument.h"
#include "text_util/ScoredWord.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::text_util;


TEST(TestDocumentAccumulator, Simple) {
  DocumentAccumulator accumulator;
  ProcessedDocument doc1("doc-1",
    vector<ScoredWord>{ ScoredWord("foo", 3, 0.5), ScoredWord("bar", 3, 0.5) },
    1.0
  );
  ProcessedDocument doc2("doc-2",
    vector<ScoredWord>{ ScoredWord("cat", 3, 0.5), ScoredWord("bar", 3, 0.5) },
    1.0
  );
  accumulator.addDocument(&doc1);
  accumulator.addDocument(&doc2);
  auto magnitude = accumulator.getMagnitude();
  EXPECT_TRUE(magnitude > 1.0);
  auto count = accumulator.getCount();
  EXPECT_EQ(2, accumulator.getCount());
  auto scores = accumulator.getScores();
  set<string> expectedWords {"foo", "bar", "cat"};
  set<string> presentWords;
  for (auto &scorePair : scores) {
    presentWords.insert(scorePair.first);
  }
  EXPECT_EQ(expectedWords, presentWords);
  EXPECT_TRUE(scores["bar"] > scores["cat"]);
}
