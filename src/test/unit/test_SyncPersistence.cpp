#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>

#include "models/Centroid.h"
#include "models/Document.h"
#include "models/ProcessedDocument.h"
#include "persistence/InMemoryRockHandle.h"
#include "persistence/RockHandle.h"
#include "persistence/SyncPersistence.h"
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "TestHelpers.h"
#include "tokenizer/Tokenizer.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;

using util::UniquePointer;
using ::testing::Return;
using ::testing::_;

#include "MockRock.h"

TEST(SyncPersistence, DoesDocumentExistTrue) {
  MockRock mockRock;
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  string key {"documents:doc-id"};
  EXPECT_CALL(mockRock, exists(key))
    .WillOnce(Return(true));
  SyncPersistence dbHandle(std::move(rockHandle));
  EXPECT_TRUE(dbHandle.doesDocumentExist("doc-id"));
}

TEST(SyncPersistence, DoesDocumentExistFalse) {
  MockRock mockRock;
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  string key {"documents:doc-id"};
  EXPECT_CALL(mockRock, exists(key))
    .WillOnce(Return(false));
  SyncPersistence dbHandle(std::move(rockHandle));
  EXPECT_FALSE(dbHandle.doesDocumentExist("doc-id"));
}

TEST(SyncPersistence, SaveDocument) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));

  double magnitude = 10.8;
  ProcessedDocument doc(
    "doc-id",
    map<string, double> { {"dog", 1.3}, {"cat", 2.6} },
    magnitude
  );
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  dbHandle.saveDocument(&doc);
  EXPECT_TRUE(mockRock.exists("documents:doc-id"));
}

TEST(SyncPersistence, DeleteDocumentExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("documents:doc-id", "something");
  EXPECT_TRUE(mockRock.exists("documents:doc-id"));
  auto res = dbHandle.deleteDocument("doc-id");
  EXPECT_TRUE(res.hasValue());
  EXPECT_EQ(true, res.value());
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
}

TEST(SyncPersistence, DeleteDocumentDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  auto res = dbHandle.deleteDocument("doc-id");
  EXPECT_TRUE(res.hasException());
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
}

TEST(SyncPersistence, LoadDocumentOptionExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  ProcessedDocument doc(
    "doc-id",
    map<string, double> {
      {"fish", 1.82},
      {"dog", 8.7}
    },
    5.6
  );
  string serialized;
  serialization::binarySerialize<ProcessedDocument>(serialized, doc);
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  mockRock.put("documents:doc-id", serialized);
  EXPECT_TRUE(mockRock.exists("documents:doc-id"));
  auto result = dbHandle.loadDocumentOption("doc-id");
  EXPECT_TRUE(result.hasValue());
  auto docPtr = result.value();
  EXPECT_EQ("doc-id", docPtr->id);
  EXPECT_EQ(2, docPtr->wordVector.scores.size());
}

TEST(SyncPersistence, LoadDocumentOptionDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));

  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  auto result = dbHandle.loadDocumentOption("doc-id");
  EXPECT_FALSE(result.hasValue());
}

TEST(SyncPersistence, LoadDocumentExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  ProcessedDocument doc(
    "doc-id",
    map<string, double> {
      {"fish", 1.82},
      {"dog", 8.7}
    },
    5.6
  );
  string serialized;
  serialization::binarySerialize<ProcessedDocument>(serialized, doc);
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  mockRock.put("documents:doc-id", serialized);
  EXPECT_TRUE(mockRock.exists("documents:doc-id"));
  auto result = dbHandle.loadDocumentOption("doc-id");
  EXPECT_TRUE(result.hasValue());
  auto docPtr = result.value();
  EXPECT_EQ("doc-id", docPtr->id);
  EXPECT_EQ(2, docPtr->wordVector.scores.size());
}

TEST(SyncPersistence, LoadDocumentDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  auto res = dbHandle.loadDocument("doc-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, DoesCentroidExistTrue) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:some-id", "anything");
  EXPECT_TRUE(dbHandle.doesCentroidExist("some-id"));
}

TEST(SyncPersistence, DoesCentroidExistFalse) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  EXPECT_FALSE(dbHandle.doesCentroidExist("some-id"));
}

TEST(SyncPersistence, DeleteCentroidSadPanda) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  auto res = dbHandle.deleteCentroid("some-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, DeleteCentroidHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:some-id", "anything");
  EXPECT_TRUE(mockRock.exists("centroids:some-id"));
  auto res = dbHandle.deleteCentroid("some-id");
  EXPECT_FALSE(res.hasException());
  EXPECT_FALSE(mockRock.exists("centroids:some-id"));
}

TEST(SyncPersistence, DeleteCentroidHappyWithDocuments) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  vector<pair<string, string>> willRetain {
    {"centroids:good-centroid-1", "anything"},
    {"centroids:good-centroid-2", "anything"},
    {"good-centroid-1__documents:doc1", "anything"},
    {"good-centroid-1__documents:doc7", "anything"},
    {"good-centroid-1__documents:doc9", "anything"},
    {"good-centroid-2__documents:doc1", "anything"},
    {"good-centroid-2__documents:doc3", "anything"},
    {"good-centroid-2__documents:doc5", "anything"}
  };
  vector<pair<string, string>> willDelete {
    {"centroids:bad-centroid", "anything"},
    {"bad-centroid__documents:doc1", "anything"},
    {"bad-centroid__documents:doc3", "anything"},
    {"bad-centroid__documents:doc8", "anything"}
  };
  for (auto &keyVal: willRetain) {
    mockRock.put(keyVal.first, keyVal.second);
  }
  for (auto &keyVal: willDelete) {
    mockRock.put(keyVal.first, keyVal.second);
  }
  EXPECT_TRUE(mockRock.exists("centroids:good-centroid-1"));
  EXPECT_TRUE(mockRock.exists("centroids:bad-centroid"));
  EXPECT_TRUE(mockRock.exists("bad-centroid__documents:doc1"));
  EXPECT_TRUE(mockRock.exists("good-centroid-1__documents:doc9"));

  dbHandle.deleteCentroid("bad-centroid");
  for (auto &keyVal: willDelete) {
    EXPECT_FALSE(mockRock.exists(keyVal.first));
  }
  for (auto &keyVal: willRetain) {
    EXPECT_TRUE(mockRock.exists(keyVal.first));
  }
}

TEST(SyncPersistence, DeleteCentroidHappyNoAssociatedDocuments) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  vector<pair<string, string>> willRetain {
    {"centroids:good-centroid-1", "anything"},
    {"centroids:good-centroid-2", "anything"},
    {"good-centroid-1__documents:doc1", "anything"},
    {"good-centroid-1__documents:doc7", "anything"},
    {"good-centroid-1__documents:doc9", "anything"},
    {"good-centroid-2__documents:doc1", "anything"},
    {"good-centroid-2__documents:doc3", "anything"},
    {"good-centroid-2__documents:doc5", "anything"}
  };
  vector<pair<string, string>> willDelete {
    {"centroids:bad-centroid", "anything"}
  };
  for (auto &keyVal: willRetain) {
    mockRock.put(keyVal.first, keyVal.second);
  }
  for (auto &keyVal: willDelete) {
    mockRock.put(keyVal.first, keyVal.second);
  }
  EXPECT_TRUE(mockRock.exists("centroids:good-centroid-1"));
  EXPECT_TRUE(mockRock.exists("centroids:bad-centroid"));
  EXPECT_TRUE(mockRock.exists("good-centroid-1__documents:doc9"));

  dbHandle.deleteCentroid("bad-centroid");
  for (auto &keyVal: willDelete) {
    EXPECT_FALSE(mockRock.exists(keyVal.first));
  }
  for (auto &keyVal: willRetain) {
    EXPECT_TRUE(mockRock.exists(keyVal.first));
  }
}

TEST(SyncPersistence, SaveCentroid) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  Centroid centroid(
    "centroid-id",
    map<string, double> {
      {"moose", 1.7},
      {"blarg", 2.21}
    },
    5.8
  );
  EXPECT_FALSE(mockRock.exists("centroids:centroid-id"));
  auto res = dbHandle.saveCentroid("centroid-id", &centroid);
  EXPECT_FALSE(res.hasException());
  auto serialized = mockRock.get("centroids:centroid-id");
  Centroid deserialized;
  serialization::binaryDeserialize<Centroid>(serialized, &deserialized);
  EXPECT_EQ("centroid-id", deserialized.id);
  EXPECT_EQ(5.8, deserialized.wordVector.magnitude);
  EXPECT_EQ(2, deserialized.wordVector.scores.size());
  EXPECT_EQ(2.21, deserialized.wordVector.scores["blarg"]);
}

TEST(SyncPersistence, LoadCentroidDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  auto res = dbHandle.loadCentroid("centroid-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, LoadCentroidExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  Centroid toSerialize(
    "centroid-id",
    map<string, double> {
      {"moose", 1.7},
      {"blarg", 2.21}
    },
    5.8
  );
  string data;
  serialization::binarySerialize<Centroid>(data, toSerialize);
  mockRock.put("centroids:centroid-id", data);
  auto res = dbHandle.loadCentroid("centroid-id");
  EXPECT_FALSE(res.hasException());
  auto centroidPtr = res.value();
  EXPECT_EQ("centroid-id", centroidPtr->id);
  EXPECT_EQ(5.8, centroidPtr->wordVector.magnitude);
  EXPECT_EQ(2, centroidPtr->wordVector.scores.size());
  EXPECT_EQ(2.21, centroidPtr->wordVector.scores["blarg"]);
}

TEST(SyncPersistence, LoadCentroidOptionExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  Centroid toSerialize(
    "centroid-id",
    map<string, double> {
      {"moose", 1.7},
      {"blarg", 2.21}
    },
    5.8
  );
  string data;
  serialization::binarySerialize<Centroid>(data, toSerialize);
  mockRock.put("centroids:centroid-id", data);
  auto res = dbHandle.loadCentroidOption("centroid-id");
  EXPECT_TRUE(res.hasValue());
  auto centroidPtr = res.value();
  EXPECT_EQ("centroid-id", centroidPtr->id);
  auto wordVec = centroidPtr->wordVector;
  EXPECT_EQ(5.8, wordVec.magnitude);
  EXPECT_EQ(2, wordVec.scores.size());
  EXPECT_EQ(2.21, wordVec.scores["blarg"]);
}

TEST(SyncPersistence, LoadCentroidOptionDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  auto res = dbHandle.loadCentroidOption("centroid-id");
  EXPECT_FALSE(res.hasValue());
}

TEST(SyncPersistence, ListAllCentroids) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  vector<string> centroidsIds {
    "centroids:some-centroid-1",
    "centroids:some-centroid-6",
    "centroids:some-centroid-3",
    "centroids:some-centroid-4",
    "centroids:some-centroid-2"
  };
  for (auto &elem: centroidsIds) {
    mockRock.put(elem, "anything");
  }
  auto centroids = dbHandle.listAllCentroids();
  vector<string> expected {
    "some-centroid-1",
    "some-centroid-2",
    "some-centroid-3",
    "some-centroid-4",
    "some-centroid-6"
  };
  EXPECT_EQ(expected, centroids);
}

TEST(SyncPersistence, AddDocumentToCentroidHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  mockRock.put("documents:document-id", "x");
  auto res = dbHandle.addDocumentToCentroid("centroid-id", "document-id");
  EXPECT_FALSE(res.hasException());
  EXPECT_TRUE(res.value());
  EXPECT_TRUE(mockRock.exists("centroid-id__documents:document-id"));
}

TEST(SyncPersistence, AddDocumentToCentroidMissingCentroid) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:unrelated-centroid-id", "x");
  mockRock.put("documents:document-id", "x");
  auto res = dbHandle.addDocumentToCentroid("centroid-id", "document-id");
  EXPECT_TRUE(res.hasException());
  EXPECT_FALSE(mockRock.exists("centroid-id__documents:document-id"));
}

TEST(SyncPersistence, AddDocumentToCentroidMissingDocument) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  mockRock.put("documents:unrelated-document-id", "x");
  auto res = dbHandle.addDocumentToCentroid("centroid-id", "document-id");
  EXPECT_TRUE(res.hasException());
  EXPECT_FALSE(mockRock.exists("centroid-id__documents:document-id"));
}

TEST(SyncPersistence, RemoveDocumentFromCentroidHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  mockRock.put("documents:document-id", "x");
  mockRock.put("centroid-id__documents:document-id", "1");
  EXPECT_TRUE(mockRock.exists("centroid-id__documents:document-id"));
  auto res = dbHandle.removeDocumentFromCentroid("centroid-id", "document-id");
  EXPECT_FALSE(res.hasException());
  EXPECT_TRUE(res.value());
  EXPECT_FALSE(mockRock.exists("centroid-id__documents:document-id"));
}

TEST(SyncPersistence, RemoveDocumentFromCentroidMissingCentroid) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:other-centroid-id", "x");
  mockRock.put("documents:document-id", "x");
  auto res = dbHandle.removeDocumentFromCentroid("centroid-id", "document-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, RemoveDocumentFromCentroidMissingDocument) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  mockRock.put("documents:unrelated-document-id", "x");
  auto res = dbHandle.removeDocumentFromCentroid("centroid-id", "document-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, ListAllDocumentsForCentroidHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  mockRock.put("centroid-id__documents:doc1", "x");
  mockRock.put("centroid-id__documents:doc2", "x");
  mockRock.put("centroid-id__documents:doc3", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_FALSE(res.hasException());
  auto docIds = res.value();
  vector<string> expected {
    "doc1", "doc2", "doc3"
  };
  EXPECT_EQ(expected, docIds);
}

TEST(SyncPersistence, ListAllDocumentsForCentroidHappyButNoDocs) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_FALSE(res.hasException());
  auto docIds = res.value();
  EXPECT_EQ(0, docIds.size());
}

TEST(SyncPersistence, ListAllDocumentsForCentroidSadPanda1) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:unrelated-centroid-id", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, ListAllDocumentsForCentroidSadPanda2) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:unrelated-centroid-id", "x");
  mockRock.put("unrelated-centroid-id__documents:some-doc", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_TRUE(res.hasException());
}

TEST(SyncPersistence, ListAllDocumentsForCentroidOptionHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  mockRock.put("centroid-id__documents:doc1", "x");
  mockRock.put("centroid-id__documents:doc2", "x");
  mockRock.put("centroid-id__documents:doc3", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_TRUE(res.hasValue());
  auto docIds = res.value();
  vector<string> expected {
    "doc1", "doc2", "doc3"
  };
  EXPECT_EQ(expected, docIds);
}

TEST(SyncPersistence, ListAllDocumentsForCentroidOptionHappyButNoDocs) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:centroid-id", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_TRUE(res.hasValue());
  auto docIds = res.value();
  EXPECT_EQ(0, docIds.size());
}

TEST(SyncPersistence, ListAllDocumentsForCentroidOptionSadPanda1) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:unrelated-centroid-id", "x");
  auto res = dbHandle.listAllDocumentsForCentroidOption("centroid-id");
  EXPECT_FALSE(res.hasValue());
}

TEST(SyncPersistence, ListAllDocumentsForCentroidOptionSadPanda2) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("centroids:unrelated-centroid-id", "x");
  mockRock.put("unrelated-centroid-id__documents:some-doc", "x");
  auto res = dbHandle.listAllDocumentsForCentroid("centroid-id");
  EXPECT_FALSE(res.hasValue());
}

TEST(SyncPersistence, ListAllDocumentsHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("documents:doc-x", "anything");
  mockRock.put("documents:doc-y", "anything");
  auto result = dbHandle.listAllDocuments();
  vector<string> expected {"doc-x", "doc-y"};
  EXPECT_EQ(expected, result);
}

TEST(SyncPersistence, ListAllDocumentsEmpty) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  auto result = dbHandle.listAllDocuments();
  vector<string> expected {};
  EXPECT_EQ(expected, result);
}

TEST(SyncPersistence, ListDocumentRangeFromIdHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("documents:doc-x1", "anything");
  mockRock.put("documents:doc-x2", "anything");
  mockRock.put("documents:doc-y1", "anything");
  mockRock.put("documents:doc-y2", "anything");
  mockRock.put("documents:doc-y3", "anything");
  auto result = dbHandle.listDocumentRangeFromId("doc-x2", 3);
  vector<string> expected {
    "doc-x2", "doc-y1", "doc-y2"
  };
  EXPECT_EQ(expected, result);
}


TEST(SyncPersistence, ListDocumentRangeFromOffsetHappy) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  SyncPersistence dbHandle(std::move(rockHandle));
  mockRock.put("documents:doc-x1", "anything");
  mockRock.put("documents:doc-x2", "anything");
  mockRock.put("documents:doc-x3", "anything");
  mockRock.put("documents:doc-y1", "anything");
  mockRock.put("documents:doc-y2", "anything");
  mockRock.put("documents:doc-y3", "anything");
  mockRock.put("documents:doc-y4", "anything");

  auto result = dbHandle.listDocumentRangeFromOffset(2, 4);
  vector<string> expected {
    "doc-x3", "doc-y1", "doc-y2", "doc-y3"
  };
  EXPECT_EQ(expected, result);
}
