#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <memory>
#include "TestHelpers.h"
#include "models/Document.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "models/ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "tokenizer/Tokenizer.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using relevanced::stopwords::StopwordFilterIf;
using relevanced::stemmer::StemmerIf;
using relevanced::tokenizer::TokenizerIf;
using ::testing::Return;
using ::testing::_;

class MockTokenizer: public TokenizerIf {
public:
  MOCK_METHOD1(tokenize, vector<string>(const string &text));
};

class MockStopwordFilter: public StopwordFilterIf {
public:
  MOCK_METHOD1(isStopword, bool(const string &text));
};

class MockStemmer: public StemmerIf {
public:
  MOCK_METHOD1(stem, string(const string &text));
  MOCK_METHOD1(stemInPlace, void(string &text));

};

TEST(DocumentProcessor, Simple) {
  vector<string> tokens = {
    "this", "is", "some", "text"
  };
  MockTokenizer tokenizer_;
  shared_ptr<TokenizerIf> tokenizer(
    &tokenizer_,
    NonDeleter<TokenizerIf>()
  );
  EXPECT_CALL(tokenizer_, tokenize("this is some text"))
    .WillOnce(Return(tokens));

  MockStemmer stemmer_;
  shared_ptr<StemmerIf> stemmer(
    &stemmer_,
    NonDeleter<StemmerIf>()
  );
  EXPECT_CALL(stemmer_, stemInPlace(tokens.at(0)));
  EXPECT_CALL(stemmer_, stemInPlace(tokens.at(1)));
  EXPECT_CALL(stemmer_, stemInPlace(tokens.at(2)));
  EXPECT_CALL(stemmer_, stemInPlace(tokens.at(3)));

  MockStopwordFilter stopwordFilter_;
  shared_ptr<StopwordFilterIf> stopwordFilter(
    &stopwordFilter_,
    NonDeleter<StopwordFilterIf>()
  );
  EXPECT_CALL(stopwordFilter_, isStopword(tokens.at(0)))
    .WillOnce(Return(false));
  EXPECT_CALL(stopwordFilter_, isStopword(tokens.at(1)))
    .WillOnce(Return(true));
  EXPECT_CALL(stopwordFilter_, isStopword(tokens.at(2)))
    .WillOnce(Return(false));
  EXPECT_CALL(stopwordFilter_, isStopword(tokens.at(3)))
    .WillOnce(Return(false));
  DocumentProcessor processor(tokenizer, stemmer, stopwordFilter);
  Document toProcess("doc-id", "this is some text");
  auto result = processor.process(toProcess);
  EXPECT_EQ("doc-id", result.id);
  EXPECT_EQ(3, result.wordVector.scores.size());
  auto counts = &result.wordVector.scores;
  EXPECT_TRUE(counts->find("this") != counts->end());
  EXPECT_TRUE(counts->find("some") != counts->end());
  EXPECT_TRUE(counts->find("text") != counts->end());
}
