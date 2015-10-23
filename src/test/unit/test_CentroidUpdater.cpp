#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <set>
#include <string>
#include <chrono>
#include <memory>

#include <folly/ExceptionWrapper.h>
#include <folly/futures/Try.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "persistence/Persistence.h"
#include "persistence/SyncPersistence.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "TestHelpers.h"
#include "centroid_update_worker/CentroidUpdater.h"
#include "centroid_update_worker/CentroidUpdaterFactory.h"
#include "centroid_update_worker/CentroidUpdateWorker.h"
#include "document_processing_worker/DocumentProcessingWorker.h"
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"

#include "text_util/ScoredWord.h"
#include "util/Clock.h"

#include "MockClock.h"
#include "MockCentroidMetadataDb.h"
#include "centroid_update_worker/DocumentAccumulatorFactory.h"

using namespace std;
using namespace wangle;
using namespace folly;
using namespace relevanced;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::document_processing_worker;
using namespace relevanced::models;
using namespace relevanced::text_util;
using namespace relevanced::util;


using thrift_protocol::ECentroidDoesNotExist;
using ::testing::Return;
using ::testing::_;

class StubSyncPersistence : public persistence::SyncPersistenceIf {
 public:
  vector<string> centroidIds;
  set<string> existingCentroids;
  map<string, ProcessedDocument*> documents;
  shared_ptr<Centroid> savedCentroid;

  Try<bool> saveCentroid(const string&, shared_ptr<Centroid> centroid) {
    savedCentroid = centroid;
    return Try<bool>(true);
  }

  Optional<shared_ptr<ProcessedDocument>> loadDocumentOption(const string& id) {
    Optional<shared_ptr<ProcessedDocument>> result;
    if (documents.find(id) != documents.end()) {
      auto docPtr = documents[id];
      result.assign(shared_ptr<ProcessedDocument>(docPtr, NonDeleter<ProcessedDocument>()));
    }
    return result;
  }
  Optional<vector<string>> listCentroidDocumentRangeFromOffsetOption(const string &id, size_t offset, size_t length) {
    Optional<vector<string>> result;
    if (existingCentroids.find(id) != existingCentroids.end()) {
      vector<string> ids;
      size_t last = offset + length;
      if (last > centroidIds.size()) {
        last = centroidIds.size();
      }
      for (size_t i = offset; i < last; i++) {
        ids.push_back(centroidIds.at(i));
      }
      result.assign(ids);
    }
    return result;
  }
  Optional<vector<string>> listCentroidDocumentRangeFromDocumentIdOption(const string &centroidId, const string &docId, size_t length) {
    Optional<vector<string>> result;
    if (existingCentroids.find(centroidId) != existingCentroids.end()) {
      size_t startingIndex = 0;
      bool found = false;
      for(size_t i = 0; i < centroidIds.size(); i++) {
        if (centroidIds.at(i) == docId) {
          found = true;
          startingIndex = i;
          break;
        }
      }
      if (found) {
        return listCentroidDocumentRangeFromOffsetOption(centroidId, startingIndex, length);
      }
      vector<string> ids;
      result.assign(ids);
    }
    return result;
  }
  MOCK_METHOD1(doesDocumentExist, bool(const string&));
  MOCK_METHOD1(saveDocument, Try<bool>(shared_ptr<ProcessedDocument>));
  MOCK_METHOD1(deleteDocument, Try<bool>(const string&));
  MOCK_METHOD0(listAllDocuments, vector<string>(void));
  MOCK_METHOD2(listDocumentRangeFromId, vector<string>(const string&, size_t));
  MOCK_METHOD2(listDocumentRangeFromOffset, vector<string>(size_t, size_t));

  MOCK_METHOD1(loadDocument, Try<shared_ptr<ProcessedDocument>>(const string&));

  MOCK_METHOD1(doesCentroidExist, bool(const string&));
  MOCK_METHOD1(createNewCentroid, Try<bool>(const string&));
  MOCK_METHOD1(deleteCentroid, Try<bool>(const string&));
  MOCK_METHOD1(loadCentroid, Try<shared_ptr<Centroid>>(const string&));
  Optional<UniquePointer<Centroid>> loadCentroidUniqueOption(const string& id) {
    Optional<UniquePointer<Centroid>> result;
    return result;
  }
  MOCK_METHOD1(loadCentroidOption,
               Optional<shared_ptr<Centroid>>(const string&));
  MOCK_METHOD0(listAllCentroids, vector<string>(void));
  MOCK_METHOD2(listCentroidRangeFromOffset, vector<string>(size_t, size_t));
  MOCK_METHOD2(listCentroidRangeFromId, vector<string>(const string&, size_t));

  MOCK_METHOD2(addDocumentToCentroid, Try<bool>(const string&, const string&));
  MOCK_METHOD2(removeDocumentFromCentroid,
               Try<bool>(const string&, const string&));
  MOCK_METHOD2(doesCentroidHaveDocument,
               Try<bool>(const string&, const string&));
  MOCK_METHOD1(listAllDocumentsForCentroid, Try<vector<string>>(const string&));
  MOCK_METHOD1(listAllDocumentsForCentroidOption,
               Optional<vector<string>>(const string&));
  MOCK_METHOD3(listCentroidDocumentRangeFromOffset,
               Try<vector<string>>(const string&, size_t, size_t));

  MOCK_METHOD3(listCentroidDocumentRangeFromDocumentId,
               Try<vector<string>>(const string&, const string&, size_t));
  MOCK_METHOD2(getCentroidMetadata,
               Optional<string>(const string&, const string&));
  MOCK_METHOD3(setCentroidMetadata,
               Try<bool>(const string&, const string&, string));
  MOCK_METHOD0(debugEraseAllData, void());
};

class MockAccumulatorFactory: public DocumentAccumulatorFactoryIf {
public:
  DocumentAccumulatorIf *toGet;
  void set(DocumentAccumulatorIf *ptr) {
    toGet = ptr;
  }
  UniquePointer<DocumentAccumulatorIf> get() {
    return UniquePointer<DocumentAccumulatorIf>(toGet, NonDeleter<DocumentAccumulatorIf>());
  }
};

class SpyAccumulator: public DocumentAccumulatorIf {
public:
  map<string, double> scores;
  set<string> seenDocumentIds;
  double magnitude {0.0};
  size_t count = 0;
  void addDocument(ProcessedDocument *doc) override {
    seenDocumentIds.insert(doc->id);
  }
  std::map<string, double>&& getScores() override {
    return std::move(scores);
  }
  size_t getCount() override {
    return count;
  }
  double getMagnitude() override {
    return magnitude;
  }
};

CentroidUpdater makeUpdater(StubSyncPersistence &stubPersistence, MockCentroidMetadataDb &mockMeta, MockClock &mockClock, MockAccumulatorFactory &fact, const string &centroidId) {
  UniquePointer<SyncPersistenceIf> syncPtr(
    &stubPersistence, NonDeleter<SyncPersistenceIf>()
  );
  auto threadPool = std::make_shared<FutureExecutor<CPUThreadPoolExecutor>>(2);
  auto persistence = make_shared<Persistence>(std::move(syncPtr), threadPool);
  shared_ptr<CentroidMetadataDbIf> metaDbPtr(
    &mockMeta, NonDeleter<CentroidMetadataDbIf>()
  );
  shared_ptr<ClockIf> clockPtr(&mockClock, NonDeleter<ClockIf>());
  shared_ptr<DocumentAccumulatorFactoryIf> accumulatorFactory(
    &fact, NonDeleter<DocumentAccumulatorFactoryIf>()
  );
  return CentroidUpdater(persistence, metaDbPtr, clockPtr, accumulatorFactory, centroidId);
}

TEST(CentroidUpdater, Simple) {
  StubSyncPersistence stubPersistence;
  map<string, ProcessedDocument> documents;
  vector<string> docIds {"doc1", "doc2", "doc3"};
  for (auto &id: docIds) {
    vector<ScoredWord> words {
      ScoredWord("cat", 3, 0.4),
      ScoredWord("dog", 3, 0.2)
    };
    documents.insert(make_pair(id, ProcessedDocument(id, words, 5.0)));
  }
  for (auto &docIdPair: documents) {
    stubPersistence.documents.insert(
      make_pair(docIdPair.first, &docIdPair.second)
    );
    stubPersistence.centroidIds.push_back(docIdPair.first);
  }
  stubPersistence.existingCentroids.insert("some-centroid");
  MockClock mclock;
  MockCentroidMetadataDb mockMeta;
  SpyAccumulator accumulator;
  MockAccumulatorFactory factory;
  factory.set(&accumulator);
  EXPECT_CALL(stubPersistence, doesCentroidExist("some-centroid"))
    .WillOnce(Return(true));
  EXPECT_CALL(mclock, getEpochTime())
    .WillOnce(Return(5555));
  CentroidUpdater updater = makeUpdater(stubPersistence, mockMeta, mclock, factory, "some-centroid");
  auto result = updater.run();
  EXPECT_FALSE(result.hasException());
  auto saved = stubPersistence.savedCentroid;
  EXPECT_EQ("some-centroid", saved->id);

  auto updateTime = mockMeta.getLastCalculatedTimestamp("some-centroid").get();
  EXPECT_TRUE(updateTime.hasValue());
  EXPECT_EQ(5555, updateTime.value());
}

TEST(CentroidUpdater, AccumulatorDetails) {
  StubSyncPersistence stubPersistence;
  map<string, ProcessedDocument> documents;
  vector<string> docIds {"doc1", "doc2", "doc3"};
  for (auto &id: docIds) {
    vector<ScoredWord> words {
      ScoredWord("cat", 3, 0.4),
      ScoredWord("dog", 3, 0.2)
    };
    documents.insert(make_pair(id, ProcessedDocument(id, words, 5.0)));
  }
  for (auto &docIdPair: documents) {
    stubPersistence.documents.insert(
      make_pair(docIdPair.first, &docIdPair.second)
    );
    stubPersistence.centroidIds.push_back(docIdPair.first);
  }
  stubPersistence.existingCentroids.insert("some-centroid");
  MockClock mclock;
  MockCentroidMetadataDb mockMeta;
  SpyAccumulator accumulator;
  MockAccumulatorFactory factory;
  factory.set(&accumulator);
  EXPECT_CALL(stubPersistence, doesCentroidExist("some-centroid"))
    .WillOnce(Return(true));
  EXPECT_CALL(mclock, getEpochTime())
    .WillOnce(Return(5555));

  map<string, double> scores {
    {"foo", 4.3},
    {"bar", 1.2}
  };
  accumulator.scores = scores;
  accumulator.magnitude = 17.5;

  CentroidUpdater updater = makeUpdater(stubPersistence, mockMeta, mclock, factory, "some-centroid");
  auto result = updater.run();
  EXPECT_FALSE(result.hasException());
  auto saved = stubPersistence.savedCentroid;
  EXPECT_EQ(17.5, saved->wordVector.magnitude);
  set<string> expectedDocs {"doc1", "doc2", "doc3"};
  EXPECT_EQ(expectedDocs, accumulator.seenDocumentIds);
}


TEST(CentroidUpdater, MissingCentroid) {
  StubSyncPersistence stubPersistence;
  MockClock mclock;
  MockCentroidMetadataDb mockMeta;
  SpyAccumulator accumulator;
  MockAccumulatorFactory factory;
  factory.set(&accumulator);

  EXPECT_CALL(stubPersistence, doesCentroidExist("some-centroid"))
    .WillOnce(Return(false));
  CentroidUpdater updater = makeUpdater(stubPersistence, mockMeta, mclock, factory, "some-centroid");
  auto result = updater.run();
  EXPECT_TRUE(result.hasException<ECentroidDoesNotExist>());

}

TEST(CentroidUpdater, MissingDocument) {
  StubSyncPersistence stubPersistence;
  map<string, ProcessedDocument> documents;
  vector<string> docIds {"doc1", "doc2", "doc3"};
  for (auto &id: docIds) {
    vector<ScoredWord> words {
      ScoredWord("cat", 3, 0.4),
      ScoredWord("dog", 3, 0.2)
    };
    documents.insert(make_pair(id, ProcessedDocument(id, words, 5.0)));
  }
  for (auto &docIdPair: documents) {
    stubPersistence.documents.insert(
      make_pair(docIdPair.first, &docIdPair.second)
    );
  }
  stubPersistence.centroidIds.push_back("doc1");
  stubPersistence.centroidIds.push_back("bad-doc");
  stubPersistence.centroidIds.push_back("doc2");
  stubPersistence.centroidIds.push_back("doc3");
  stubPersistence.existingCentroids.insert("some-centroid");
  MockClock mclock;
  MockCentroidMetadataDb mockMeta;
  SpyAccumulator accumulator;
  MockAccumulatorFactory factory;
  factory.set(&accumulator);

  EXPECT_CALL(stubPersistence, doesCentroidExist("some-centroid"))
    .WillOnce(Return(true));
  EXPECT_CALL(mclock, getEpochTime())
    .WillOnce(Return(5555));

  Try<bool> removeResponse(true);
  EXPECT_CALL(stubPersistence, removeDocumentFromCentroid("some-centroid", "bad-doc"))
    .WillOnce(Return(removeResponse));
  CentroidUpdater updater = makeUpdater(stubPersistence, mockMeta, mclock, factory, "some-centroid");
  auto result = updater.run();
  EXPECT_FALSE(result.hasException());
  auto saved = stubPersistence.savedCentroid;
  EXPECT_EQ("some-centroid", saved->id);
  auto updateTime = mockMeta.getLastCalculatedTimestamp("some-centroid").get();
  EXPECT_TRUE(updateTime.hasValue());
  EXPECT_EQ(5555, updateTime.value());
}