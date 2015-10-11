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
#include "tokenizer/Tokenizer.h"
#include "TestHelpers.h"
#include "MockHasher.h"
#include "util/Sha1Hasher.h"

using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using namespace relevanced::util;
using relevanced::stopwords::StopwordFilterIf;
using relevanced::stemmer::StemmerIf;
using relevanced::tokenizer::TokenizerIf;
using ::testing::Return;
using ::testing::_;

class MockDocumentProcessor: public DocumentProcessorIf {
public:
  MOCK_METHOD1(process, ProcessedDocument(const Document&));
  MOCK_METHOD1(processNew, shared_ptr<ProcessedDocument>(const Document&));
  MOCK_METHOD1(processNew, shared_ptr<ProcessedDocument>(shared_ptr<Document>));
};

TEST(DocumentProcessingWorker, Simple) {
  auto threadPool = make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  MockDocumentProcessor mockProcessor;

  shared_ptr<DocumentProcessorIf> processorPtr(
    &mockProcessor, NonDeleter<DocumentProcessorIf>()
  );

  MockHasher hasher;
  shared_ptr<Sha1HasherIf> hasherPtr(
    &hasher, NonDeleter<Sha1HasherIf>()
  );

  DocumentProcessingWorker worker(
    processorPtr,
    hasherPtr,
    threadPool
  );

  shared_ptr<ProcessedDocument> processed = make_shared<ProcessedDocument>(
    "processed-doc-id",
    map<string, double>{
      {"some", 1.3},
      {"bears", 9.6}
    },
    20.3
  );

  shared_ptr<Document> doc = make_shared<Document>(
    "doc-id",
    "This is some text about bears"
  );

  EXPECT_CALL(mockProcessor, processNew(doc))
    .WillOnce(Return(processed));

  auto result = worker.processNew(doc).get();
  EXPECT_EQ(processed, result);
}
