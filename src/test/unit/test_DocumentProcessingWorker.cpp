#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <memory>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "TestHelpers.h"
#include "Document.h"
#include "DocumentProcessor.h"
#include "DocumentProcessingWorker.h"
#include "ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "tokenizer/Tokenizer.h"

using namespace std;
using namespace wangle;
using stopwords::StopwordFilterIf;
using stemmer::StemmerIf;
using tokenizer::TokenizerIf;
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

  DocumentProcessingWorker worker(
    processorPtr,
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
