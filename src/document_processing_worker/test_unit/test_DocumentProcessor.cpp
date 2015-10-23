#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <memory>
#include "testing/TestHelpers.h"
#include "models/Document.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "models/ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/Utf8Stemmer.h"

#include "util/Clock.h"
#include "testing/MockClock.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using namespace relevanced::util;
using relevanced::stopwords::StopwordFilterIf;
using relevanced::stemmer::Utf8Stemmer;
using relevanced::stemmer::StemmerIf;

using ::testing::Return;
using ::testing::_;

class MockStopwordFilter : public StopwordFilterIf {
 public:
  bool isStopword(const string&) {
    return false;
  }
};

class NonStemmer: public StemmerIf {
public:
  size_t getStemPos(const char*, size_t length) {
    return length;
  }
};

TEST(DocumentProcessor, Simple) {

  MockStopwordFilter stopwordFilter_;
  shared_ptr<StopwordFilterIf> stopwordFilter(&stopwordFilter_,
                                              NonDeleter<StopwordFilterIf>());

  MockClock mClock;
  shared_ptr<ClockIf> clockPtr(&mClock, NonDeleter<ClockIf>());

  EXPECT_CALL(mClock, getEpochTime()).WillOnce(Return(1234));

  DocumentProcessor processor(make_shared<NonStemmer>(), stopwordFilter, clockPtr);
  Document toProcess("doc-id", "some text about fish fish text");
  auto result = processor.process(toProcess);
  EXPECT_EQ("doc-id", result.id);
  EXPECT_EQ(4, result.scoredWords.size());
  EXPECT_EQ(1234, result.created);
  EXPECT_EQ(1234, result.updated);

}
