#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "models/Document.h"
#include "models/ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "testing/TestHelpers.h"
#include "testing/MockHasher.h"
#include "util/Sha1Hasher.h"
#include "text_util/ScoredWord.h"
#include "gen-cpp2/RelevancedProtocol_types.h"

using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using namespace relevanced::util;
using namespace relevanced::text_util;
using relevanced::stopwords::StopwordFilterIf;
using relevanced::stemmer::StemmerIf;
using relevanced::thrift_protocol::Language;
using ::testing::Return;
using ::testing::_;

class MockDocumentProcessor : public DocumentProcessorIf {
 public:
  MOCK_METHOD1(process, ProcessedDocument(Document&));
  MOCK_METHOD1(processNew, shared_ptr<ProcessedDocument>(Document&));
  MOCK_METHOD1(processNew, shared_ptr<ProcessedDocument>(shared_ptr<Document>));

};

TEST(DocumentProcessingWorker, Simple) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockDocumentProcessor mockProcessor;

  shared_ptr<DocumentProcessorIf> processorPtr(
      &mockProcessor, NonDeleter<DocumentProcessorIf>());

  MockHasher hasher;
  shared_ptr<Sha1HasherIf> hasherPtr(&hasher, NonDeleter<Sha1HasherIf>());

  DocumentProcessingWorker worker(processorPtr, hasherPtr, threadPool);

  vector<ScoredWord> words {
    ScoredWord("some", 4, 1.3),
    ScoredWord("bears", 5, 9.6)
  };

  shared_ptr<ProcessedDocument> processed = make_shared<ProcessedDocument>(
    "processed-doc-id", words, 20.3
  );
  shared_ptr<Document> doc = make_shared<Document>(
    "doc-id", "This is some text about bears", Language::EN
  );

  EXPECT_CALL(hasher, hash("This is some text about bears"))
      .WillOnce(Return("SHA1_HASH"));

  EXPECT_CALL(mockProcessor, processNew(doc)).WillOnce(Return(processed));

  auto result = worker.processNew(doc).get();
  EXPECT_EQ(processed, result);
  EXPECT_TRUE(processed->sha1Hash.hasValue());
  EXPECT_EQ("SHA1_HASH", processed->sha1Hash.value());
}

TEST(DocumentProcessingWorker, WithoutHash) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockDocumentProcessor mockProcessor;

  shared_ptr<DocumentProcessorIf> processorPtr(
      &mockProcessor, NonDeleter<DocumentProcessorIf>());

  StupidMockHasher hasher;
  shared_ptr<Sha1HasherIf> hasherPtr(&hasher, NonDeleter<Sha1HasherIf>());

  DocumentProcessingWorker worker(processorPtr, hasherPtr, threadPool);

  vector<ScoredWord> words {
    ScoredWord("some", 4, 1.3),
    ScoredWord("bears", 5, 9.6)
  };

  shared_ptr<ProcessedDocument> processed = make_shared<ProcessedDocument>(
    "processed-doc-id", words, 20.3
  );
  shared_ptr<Document> doc = make_shared<Document>(
    "doc-id", "This is some text about bears", Language::EN
  );

  EXPECT_CALL(mockProcessor, processNew(doc)).WillOnce(Return(processed));

  auto result = worker.processNewWithoutHash(doc).get();
  EXPECT_EQ(processed, result);
  EXPECT_FALSE(processed->sha1Hash.hasValue());
}
