#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/Optional.h>
#include <folly/futures/Try.h>

#include "TestHelpers.h"
#include "Document.h"
#include "Centroid.h"

#include "DocumentProcessor.h"
#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "SimilarityScoreWorker.h"
#include "ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "tokenizer/Tokenizer.h"
#include "MockSyncPersistence.h"
#include "util.h"

using namespace std;
using namespace wangle;
using namespace persistence;
using persistence::exceptions::CentroidDoesNotExist;
using stopwords::StopwordFilterIf;
using stemmer::StemmerIf;
using tokenizer::TokenizerIf;
using util::UniquePointer;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

shared_ptr<SimilarityScoreWorker> makeWorker(MockSyncPersistence &syncPersistence) {
  UniquePointer<SyncPersistenceIf> syncPersistencePtr(
    &syncPersistence, NonDeleter<SyncPersistenceIf>()
  );
  auto threadPool1 = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  auto threadPool2 = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);

  shared_ptr<PersistenceIf> persistencePtr(
    new Persistence(std::move(syncPersistencePtr), threadPool1)
  );
  return make_shared<SimilarityScoreWorker>(
    persistencePtr, threadPool2
  );
}

TEST(SimilarityScoreWorker, TestInitialization) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);
  vector<string> centroidIds {"centroid-1", "centroid-2"};
  EXPECT_CALL(mockPersistence, listAllCentroids())
    .WillOnce(Return(centroidIds));

  Centroid c1(
    "centroid-1",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    5.3
  );
  Centroid c2(
    "centroid-2",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    5.3
  );
  Optional<shared_ptr<Centroid>> c1Opt(
    shared_ptr<Centroid>(&c1, NonDeleter<Centroid>())
  );
  Optional<shared_ptr<Centroid>> c2Opt(
    shared_ptr<Centroid>(&c2, NonDeleter<Centroid>())
  );


  EXPECT_CALL(mockPersistence, loadCentroidOption(_))
    .WillOnce(Return(c1Opt))
    .WillOnce(Return(c2Opt));

  worker->initialize();

  auto result1 = worker->getLoadedCentroid_("centroid-1");
  EXPECT_TRUE(result1.hasValue());
  EXPECT_EQ("centroid-1", result1.value()->id);

  auto result2 = worker->getLoadedCentroid_("centroid-2");
  EXPECT_TRUE(result2.hasValue());
  EXPECT_EQ("centroid-2", result2.value()->id);

}

TEST(SimilarityScoreWorker, TestGetSetLoaded) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);
  Centroid c1(
    "centroid-1",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    5.3
  );
  shared_ptr<Centroid> c1Ptr(&c1, NonDeleter<Centroid>());
  worker->setLoadedCentroid_("centroid-1", c1Ptr);

  auto res = worker->getLoadedCentroid_("centroid-1");
  EXPECT_TRUE(res.hasValue());
  EXPECT_EQ(c1Ptr, res.value());
}

TEST(SimilarityScoreWorker, TestGetLoadedCentroidMissing) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);
  Centroid c1(
    "irrelevant-centroid",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    5.3
  );
  shared_ptr<Centroid> c1Ptr(&c1, NonDeleter<Centroid>());
  worker->setLoadedCentroid_("irrelevant-centroid", c1Ptr);

  auto res = worker->getLoadedCentroid_("centroid-id");
  EXPECT_FALSE(res.hasValue());
}

TEST(SimilarityScoreWorker, TestReloadCentroid) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);

  Centroid c1Old(
    "centroid-1",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    5.3
  );
  Centroid c1New(
    "centroid-1",
    map<string, double> {
      {"cat", 503.1},
      {"dog", 86.2},
      {"fish", 1378.3}
    },
    9873.9
  );

  Optional<shared_ptr<Centroid>> c1NewOption(
    shared_ptr<Centroid>(&c1New, NonDeleter<Centroid>())
  );
  EXPECT_CALL(mockPersistence, loadCentroidOption("centroid-1"))
    .WillOnce(Return(c1NewOption));

  worker->setLoadedCentroid_(
    "centroid-1",
    shared_ptr<Centroid>(&c1Old, NonDeleter<Centroid>())
  );
  EXPECT_EQ(1.2, worker->getLoadedCentroid_("centroid-1").value()->scores["cat"]);

  worker->reloadCentroid("centroid-1").get();
  EXPECT_EQ(503.1, worker->getLoadedCentroid_("centroid-1").value()->scores["cat"]);
}

double mag3(double x, double y, double z) {
  return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

TEST(SimilarityScoreWorker, TestGetDocumentSimilarityHappy) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);
  ProcessedDocument document(
    "doc-1",
    map<string, double>{
      {"dog", 5.8},
      {"fox", 4.1},
      {"sarah_jessica_parker", 15.1}
    },
    mag3(5.8, 4.1, 15.1)
  );
  Centroid c1(
    "centroid-1",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    mag3(1.2, 9.5, 0.8)
  );
  shared_ptr<Centroid> c1Ptr(&c1, NonDeleter<Centroid>());
  worker->setLoadedCentroid_("centroid-1", c1Ptr);

  auto result = worker->getDocumentSimilarity("centroid-1", &document).get();
  EXPECT_FALSE(result.hasException());
  auto score = result.value();
  EXPECT_TRUE(score > 0.0);
  EXPECT_TRUE(score < 1.0);
}

TEST(SimilarityScoreWorker, TestGetDocumentSimilarityMissingCentroid) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);
  ProcessedDocument document(
    "doc-1",
    map<string, double>{
      {"dog", 5.8},
      {"fox", 4.1},
      {"sarah_jessica_parker", 15.1}
    },
    mag3(5.8, 4.1, 15.1)
  );
  Centroid c1(
    "irrelevant-centroid",
    map<string, double> {
      {"cat", 1.2},
      {"dog", 9.5},
      {"fish", 0.8}
    },
    mag3(1.2, 9.5, 0.8)
  );
  shared_ptr<Centroid> c1Ptr(&c1, NonDeleter<Centroid>());
  worker->setLoadedCentroid_("irrelevant-centroid-1", c1Ptr);
  auto result = worker->getDocumentSimilarity("centroid-1", &document).get();
  EXPECT_TRUE(result.hasException<CentroidDoesNotExist>());
}


TEST(SimilarityScoreWorker, TestGetDocumentSimilarityNoCentroidNoNoneAtAll) {
  MockSyncPersistence mockPersistence;
  auto worker = makeWorker(mockPersistence);
  ProcessedDocument document(
    "doc-1",
    map<string, double>{
      {"dog", 5.8},
      {"fox", 4.1},
      {"sarah_jessica_parker", 15.1}
    },
    mag3(5.8, 4.1, 15.1)
  );
  auto result = worker->getDocumentSimilarity("centroid-1", &document).get();
  EXPECT_TRUE(result.hasException<CentroidDoesNotExist>());
}
