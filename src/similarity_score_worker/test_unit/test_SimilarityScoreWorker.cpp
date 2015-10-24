#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cmath>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/Optional.h>
#include <folly/futures/Try.h>
#include <glog/logging.h>
#include "testing/TestHelpers.h"
#include "models/Document.h"
#include "models/Centroid.h"

#include "document_processing_worker/DocumentProcessor.h"
#include "persistence/Persistence.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "persistence/CentroidMetadataDb.h"

#include "persistence/SyncPersistence.h"
#include "similarity_score_worker/SimilarityScoreWorker.h"
#include "models/ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "tokenizer/Tokenizer.h"
#include "testing/MockSyncPersistence.h"
#include "testing/MockCentroidMetadataDb.h"
#include "util/util.h"
#include "text_util/ScoredWord.h"


using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::util;
using namespace relevanced::models;
using namespace relevanced::similarity_score_worker;
using namespace relevanced::persistence;
using namespace relevanced::thrift_protocol;
using relevanced::thrift_protocol::ECentroidDoesNotExist;
using relevanced::stopwords::StopwordFilterIf;
using relevanced::stemmer::StemmerIf;
using relevanced::tokenizer::TokenizerIf;
using util::UniquePointer;
using text_util::ScoredWord;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

shared_ptr<SimilarityScoreWorker> makeWorker(
    MockSyncPersistence &syncPersistence, MockCentroidMetadataDb &metadata) {
  UniquePointer<SyncPersistenceIf> syncPersistencePtr(
      &syncPersistence, NonDeleter<SyncPersistenceIf>());
  auto threadPool1 = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  auto threadPool2 = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);

  shared_ptr<PersistenceIf> persistencePtr(
      new Persistence(std::move(syncPersistencePtr), threadPool1));
  shared_ptr<CentroidMetadataDbIf> metadataPtr(
      &metadata, NonDeleter<CentroidMetadataDbIf>());
  return make_shared<SimilarityScoreWorker>(persistencePtr, metadataPtr,
                                            threadPool2);
}

TEST(SimilarityScoreWorker, TestInitialization) {
  MockSyncPersistence mockPersistence;
  MockCentroidMetadataDb metadataDb;
  auto worker = makeWorker(mockPersistence, metadataDb);
  vector<string> centroidIds{"centroid-1", "centroid-2"};
  EXPECT_CALL(mockPersistence, listAllCentroids())
      .WillOnce(Return(centroidIds));

  mockPersistence.addUniqueCentroid("centroid-1", new Centroid("centroid-1",
              unordered_map<string, double>{{"cat", 1.2}, {"dog", 9.5}, {"fish", 0.8}},
              5.3));
  mockPersistence.addUniqueCentroid("centroid-2", new Centroid("centroid-2",
              unordered_map<string, double>{{"cat", 1.2}, {"dog", 9.5}, {"fish", 0.8}},
              5.3));

  worker->initialize();
  auto result1 = worker->debugGetCentroid("centroid-1");
  EXPECT_TRUE(result1.hasValue());
  EXPECT_EQ("centroid-1", result1.value()->id);

  auto result2 = worker->debugGetCentroid("centroid-2");
  EXPECT_TRUE(result2.hasValue());
  EXPECT_EQ("centroid-2", result2.value()->id);

  EXPECT_FALSE(worker->debugGetCentroid("bad-centroid").hasValue());
}

TEST(SimilarityScoreWorker, TestReloadCentroid) {
  MockSyncPersistence mockPersistence;
  MockCentroidMetadataDb metadataDb;
  auto worker = makeWorker(mockPersistence, metadataDb);

  mockPersistence.addUniqueCentroid("centroid-1", new Centroid ("centroid-1",
                 unordered_map<string, double>{{"cat", 1.2}, {"dog", 9.5}, {"fish", 0.8}},
                 5.3));

  auto nonexistent = worker->debugGetCentroid("centroid-1");
  EXPECT_FALSE(nonexistent.hasValue());

  worker->reloadCentroid("centroid-1").get();
  auto existing = worker->debugGetCentroid("centroid-1");
  EXPECT_TRUE(existing.hasValue());
  EXPECT_EQ(5.3, existing.value()->wordVector.magnitude);

  mockPersistence.addUniqueCentroid("centroid-1", new Centroid ("centroid-1",
                 unordered_map<string, double>{{"cat", 1.2}, {"dog", 9.5}, {"fish", 0.8}},
                 17.2));

  worker->reloadCentroid("centroid-1").get();
  auto reloaded = worker->debugGetCentroid("centroid-1");
  EXPECT_TRUE(reloaded.hasValue());
  EXPECT_EQ(17.2, reloaded.value()->wordVector.magnitude);
}

double mag3(double x, double y, double z) {
  return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

TEST(SimilarityScoreWorker, TestGetDocumentSimilarityHappy) {
  MockSyncPersistence mockPersistence;
  MockCentroidMetadataDb metadataDb;
  auto worker = makeWorker(mockPersistence, metadataDb);
  vector<ScoredWord> words {
    ScoredWord("dog", 3, 5.8),
    ScoredWord("fox", 3, 4.1),
    ScoredWord("sarah_jessica_parker", 20, 15.1)
  };
  ProcessedDocument document("doc-1", words, mag3(5.8, 4.1, 15.1));

  mockPersistence.addUniqueCentroid("centroid-1", new Centroid ("centroid-1",
              unordered_map<string, double>{{"cat", 1.2}, {"dog", 9.5}, {"fish", 0.8}},
              mag3(1.2, 9.5, 0.8)));
  worker->reloadCentroid("centroid-1").get();
  auto result = worker->getDocumentSimilarity("centroid-1", &document).get();
  EXPECT_FALSE(result.hasException());
  auto score = result.value();
  EXPECT_TRUE(score > 0.0);
  EXPECT_TRUE(score < 1.0);
}

TEST(SimilarityScoreWorker, TestGetDocumentSimilarityMissingCentroid) {
  MockSyncPersistence mockPersistence;
  MockCentroidMetadataDb metadataDb;
  auto worker = makeWorker(mockPersistence, metadataDb);
  vector<ScoredWord> words {
    ScoredWord("dog", 3, 5.8),
    ScoredWord("fox", 3, 4.1),
    ScoredWord("sarah_jessica_parker", 20, 15.1)
  };
  mockPersistence.addUniqueCentroid("centroid-1", new Centroid ("centroid-1",
              unordered_map<string, double>{{"cat", 1.2}, {"dog", 9.5}, {"fish", 0.8}},
              mag3(1.2, 9.5, 0.8)));
  worker->reloadCentroid("centroid-1").get();

  ProcessedDocument document("doc-1", words, mag3(5.8, 4.1, 15.1));
  auto result = worker->getDocumentSimilarity("other-centroid", &document).get();
  EXPECT_TRUE(result.hasException<ECentroidDoesNotExist>());
}

TEST(SimilarityScoreWorker, TestGetDocumentSimilarityNoCentroidNoNoneAtAll) {
  MockSyncPersistence mockPersistence;
  MockCentroidMetadataDb metadataDb;
  auto worker = makeWorker(mockPersistence, metadataDb);
  vector<ScoredWord> words {
    ScoredWord("dog", 3, 5.8),
    ScoredWord("fox", 3, 4.1),
    ScoredWord("sarah_jessica_parker", 20, 15.1)
  };
  ProcessedDocument document("doc-1", words, mag3(5.8, 4.1, 15.1));
  auto result = worker->getDocumentSimilarity("centroid-1", &document).get();
  EXPECT_TRUE(result.hasException<ECentroidDoesNotExist>());
}
