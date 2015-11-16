#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <set>
#include <string>
#include <chrono>
#include <memory>

#include <glog/logging.h>
#include <folly/ExceptionWrapper.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "testing/TestHelpers.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "persistence/CentroidMetadataDb.h"
#include "persistence/InMemoryRockHandle.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/ThreadSafeUtf8Stemmer.h"
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "models/Document.h"

#include "testing/MockSyncPersistence.h"
#include "util/util.h"
#include "util/Clock.h"
#include "util/Sha1Hasher.h"


using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;
using namespace relevanced::util;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::document_processing_worker;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::tokenizer;
using relevanced::thrift_protocol::Language;

using ::testing::Return;
using ::testing::_;


struct ProcessingWorkerTestCtx {
  shared_ptr<PersistenceIf> persistence;
  shared_ptr<Sha1HasherIf> hasher;
  shared_ptr<ClockIf> sysClock;
  shared_ptr<StemmerIf> stemmer;
  shared_ptr<StopwordFilterIf> stopwordFilter;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool1;
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool2;
  shared_ptr<DocumentProcessorIf> processor;
  shared_ptr<DocumentProcessingWorker> worker;

  ProcessingWorkerTestCtx() {
    threadPool1.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    threadPool2.reset(new wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>(2));
    UniquePointer<RockHandleIf> rockHandle(new InMemoryRockHandle("foo"));
    UniquePointer<SyncPersistenceIf> syncPersistence(
      new SyncPersistence(std::move(rockHandle))
    );
    persistence.reset(
      new Persistence(std::move(syncPersistence), threadPool1)
    );
    hasher.reset(new Sha1Hasher);
    sysClock.reset(new Clock);
    stemmer.reset(new ThreadSafeUtf8Stemmer);
    stopwordFilter.reset(new StopwordFilter);
    processor.reset(
      new DocumentProcessor(stemmer, stopwordFilter, sysClock)
    );
    worker.reset(new DocumentProcessingWorker(
      processor, hasher, threadPool2
    ));
  }
};

TEST(DocumentProcessingWorker, TestSanity) {
  ProcessingWorkerTestCtx ctx;
  Document document(
    "doc-id", "this is some document text fish words", Language::EN
  );
  shared_ptr<Document> docPtr(&document, NonDeleter<Document>());
  auto result = ctx.worker->processNew(docPtr).get();
  EXPECT_EQ("doc-id", result->id);
  set<string> words;
  for (auto &scoredWord: result->scoredWords) {
    words.insert(scoredWord.word);
  }
  EXPECT_TRUE(words.find("fish") != words.end());
}

TEST(DocumentProcessingWorker, TestWithWithoutHash) {
  ProcessingWorkerTestCtx ctx;
  Document document(
    "doc-id", "this is some document text", Language::EN
  );
  shared_ptr<Document> docPtr(&document, NonDeleter<Document>());
  auto result = ctx.worker->processNew(docPtr).get();
  EXPECT_TRUE(result->sha1Hash.hasValue());

  auto result2 = ctx.worker->processNewWithoutHash(docPtr).get();
  EXPECT_FALSE(result2->sha1Hash.hasValue());
}
