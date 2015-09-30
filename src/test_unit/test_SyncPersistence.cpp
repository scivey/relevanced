#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>

#include "util.h"
#include "TestHelpers.h"
#include "Document.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "stopwords/StopwordFilter.h"
#include "stemmer/StemmerIf.h"
#include "tokenizer/Tokenizer.h"
#include "persistence/SyncPersistence.h"
#include "persistence/RockHandle.h"
#include "persistence/InMemoryRockHandle.h"
#include "Centroid.h"

using namespace std;
using namespace persistence;

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
  auto serialized = serialization::jsonSerialize<ProcessedDocument>(&doc);
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  mockRock.put("documents:doc-id", serialized);
  EXPECT_TRUE(mockRock.exists("documents:doc-id"));
  auto result = dbHandle.loadDocumentOption("doc-id");
  EXPECT_TRUE(result.hasValue());
  auto docPtr = result.value();
  EXPECT_EQ("doc-id", docPtr->id);
  EXPECT_EQ(2, docPtr->normalizedWordCounts.size());
}

TEST(SyncPersistence, LoadDocumentOptionDNE) {
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
  auto serialized = serialization::jsonSerialize<ProcessedDocument>(&doc);
  EXPECT_FALSE(mockRock.exists("documents:doc-id"));
  mockRock.put("documents:doc-id", serialized);
  EXPECT_TRUE(mockRock.exists("documents:doc-id"));
  auto result = dbHandle.loadDocumentOption("doc-id");
  EXPECT_TRUE(result.hasValue());
  auto docPtr = result.value();
  EXPECT_EQ("doc-id", docPtr->id);
  EXPECT_EQ(2, docPtr->normalizedWordCounts.size());
}

TEST(SyncPersistence, LoadDocumentDNE) {
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

TEST(SyncPersistence, DeleteCentroidHappySimple) {
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


// TEST(SyncPersistence, LoadDocumentExists) {
//   InMemoryRockHandle mockRock("/some-path");
//   UniquePointer<RockHandleIf> rockHandle(
//     &mockRock, NonDeleter<RockHandleIf>()
//   );
//   SyncPersistence dbHandle(std::move(rockHandle));
//   map<string, double> wordCounts = {
//     {"fish", 1.82},
//     {"dog", 8.7}
//   };
//   ProcessedDocument doc("doc-id", wordCounts);

//   unsigned char *serialized;
//   size_t len = serialization::binarySerialize(&serialized, doc);
//   rocksdb::Slice data((char*) serialized, len);
//   EXPECT_FALSE(mockRock.exists("doc-id"));
//   mockRock.put("doc-id", data);
//   EXPECT_TRUE(mockRock.exists("doc-id"));

//   auto result = dbHandle.loadDocument("doc-id");

//   EXPECT_TRUE(result.hasValue());
//   auto loaded = std::move(result.value());
//   EXPECT_EQ("doc-id", loaded->id);
//   EXPECT_EQ(2, loaded->normalizedWordCounts.size());
//   free(serialized);
// }

// TEST(SyncPersistence, LoadDocumentDoesNotExist) {
//   InMemoryRockHandle mockRock("/some-path");
//   UniquePointer<RockHandleIf> rockHandle(
//     &mockRock, NonDeleter<RockHandleIf>()
//   );
//   SyncPersistence dbHandle(std::move(rockHandle));
//   EXPECT_FALSE(mockRock.exists("doc-id"));

//   auto result = dbHandle.loadDocument("doc-id");

//   EXPECT_FALSE(result.hasValue());
// }


// TEST(ClassifierDBHandle, DoesClassifierExistTrue) {
//   MockRock docMockRock;
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   MockRock listMockRock;
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string id {"classifier-id"};
//   EXPECT_CALL(listMockRock, exists(id))
//     .WillOnce(Return(true));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_TRUE(dbHandle.doesClassifierExist(id));
// }

// TEST(ClassifierDBHandle, DoesClassifierExistFalse) {
//   MockRock docMockRock;
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   MockRock listMockRock;
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string id {"classifier-id"};
//   EXPECT_CALL(listMockRock, exists(id))
//     .WillOnce(Return(false));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_FALSE(dbHandle.doesClassifierExist(id));
// }

// TEST(ClassifierDBHandle, CreateClassifierExists) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string data {"value"};
//   listMockRock.put("classifier", data);
//   EXPECT_TRUE(listMockRock.exists("classifier"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );

//   EXPECT_FALSE(dbHandle.createClassifier("classifier"));

//   EXPECT_EQ("value", listMockRock.get("classifier"));
// }

// TEST(ClassifierDBHandle, CreateClassifierDoesNotExist) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   EXPECT_FALSE(listMockRock.exists("classifier"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );

//   EXPECT_TRUE(dbHandle.createClassifier("classifier"));

//   EXPECT_TRUE(listMockRock.get("classifier") != "");
// }

// TEST(ClassifierDBHandle, DoesClassifierHaveDocumentTrue) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string val {"1"};
//   docMockRock.put("classifier_id:document_id", val);
//   EXPECT_TRUE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );

//   EXPECT_TRUE(dbHandle.doesClassifierHaveDocument("classifier_id", "document_id"));
// }

// TEST(ClassifierDBHandle, DoesClassifierHaveDocumentFalse) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );

//   EXPECT_FALSE(dbHandle.doesClassifierHaveDocument("classifier_id", "document_id"));
// }

// TEST(ClassifierDBHandle, AddPositiveDocumentToClassifierHappy) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string collVal {"1"};
//   listMockRock.put("classifier_id", collVal);
//   EXPECT_TRUE(listMockRock.exists("classifier_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_TRUE(dbHandle.addPositiveDocumentToClassifier("classifier_id", "document_id"));
//   EXPECT_EQ("1", docMockRock.get("classifier_id:document_id"));
// }

// TEST(ClassifierDBHandle, AddPositiveDocumentToClassifierMissingClassifier) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   EXPECT_FALSE(listMockRock.exists("classifier_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_FALSE(dbHandle.addPositiveDocumentToClassifier("classifier_id", "document_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
// }

// TEST(ClassifierDBHandle, AddPositiveDocumentToClassifierAlreadyExists) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string collVal {"1"};
//   listMockRock.put("classifier_id", collVal);
//   EXPECT_TRUE(listMockRock.exists("classifier_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   string existing {"existing_value"};
//   docMockRock.put("classifier_id:document_id", existing);
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_FALSE(dbHandle.addPositiveDocumentToClassifier("classifier_id", "document_id"));
//   EXPECT_EQ("existing_value", docMockRock.get("classifier_id:document_id"));
// }

// TEST(ClassifierDBHandle, AddNegativeDocumentToClassifierHappy) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string collVal {"1"};
//   listMockRock.put("classifier_id", collVal);
//   EXPECT_TRUE(listMockRock.exists("classifier_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_TRUE(dbHandle.addNegativeDocumentToClassifier("classifier_id", "document_id"));
//   EXPECT_EQ("0", docMockRock.get("classifier_id:document_id"));
// }

// TEST(ClassifierDBHandle, AddNegativeDocumentToClassifierMissingClassifier) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   EXPECT_FALSE(listMockRock.exists("classifier_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_FALSE(dbHandle.addNegativeDocumentToClassifier("classifier_id", "document_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
// }

// TEST(ClassifierDBHandle, AddNegativeDocumentToClassifierAlreadyExists) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string collVal {"1"};
//   listMockRock.put("classifier_id", collVal);
//   EXPECT_TRUE(listMockRock.exists("classifier_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   string existing {"existing_value"};
//   docMockRock.put("classifier_id:document_id", existing);
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_FALSE(dbHandle.addNegativeDocumentToClassifier("classifier_id", "document_id"));
//   EXPECT_EQ("existing_value", docMockRock.get("classifier_id:document_id"));
// }


// TEST(ClassifierDBHandle, RemoveDocFromClassifierHappy) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string keyVal {"anything"};
//   docMockRock.put("classifier_id:document_id", keyVal);
//   EXPECT_TRUE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_TRUE(dbHandle.removeDocumentFromClassifier("classifier_id", "document_id"));
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
// }

// TEST(ClassifierDBHandle, RemoveDocFromClassifierSadPanda) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   EXPECT_FALSE(dbHandle.removeDocumentFromClassifier("classifier_id", "document_id"));
// }

// TEST(ClassifierDBHandle, ListClassifiers) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string anything {"anything"};
//   listMockRock.put("coll1", anything);
//   listMockRock.put("coll2", anything);
//   listMockRock.put("coll3", anything);
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   auto classifiers = dbHandle.listClassifiers();
//   EXPECT_EQ(3, classifiers.size());
//   EXPECT_EQ("coll1", classifiers.at(0));
//   EXPECT_EQ("coll2", classifiers.at(1));
//   EXPECT_EQ("coll3", classifiers.at(2));
// }

// TEST(ClassifierDBHandle, ListClassifiersEmpty) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   auto classifiers = dbHandle.listClassifiers();
//   EXPECT_EQ(0, classifiers.size());
// }

// TEST(ClassifierDBHandle, ListAllClassifierDocuments) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string anything {"1"};
//   listMockRock.put("coll1", anything);
//   docMockRock.put("coll1:doc1", anything);
//   docMockRock.put("coll1:doc2", anything);
//   docMockRock.put("coll1:doc3", anything);
//   docMockRock.put("coll1:doc4", anything);
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   auto documents = dbHandle.listAllClassifierDocuments("coll1");
//   EXPECT_EQ(4, documents.size());
//   EXPECT_EQ("doc1", documents.at(0));
//   EXPECT_EQ("doc2", documents.at(1));
//   EXPECT_EQ("doc3", documents.at(2));
//   EXPECT_EQ("doc4", documents.at(3));
// }

// TEST(ClassifierDBHandle, ListAllClassifierDocumentsEmpty) {
//   InMemoryRockHandle docMockRock("doc");
//   UniquePointer<RockHandleIf> docRockHandle(
//     &docMockRock, NonDeleter<RockHandleIf>()
//   );
//   InMemoryRockHandle listMockRock("list");
//   UniquePointer<RockHandleIf> listRockHandle(
//     &listMockRock, NonDeleter<RockHandleIf>()
//   );
//   string anything {"1"};
//   listMockRock.put("coll1", anything);
//   ClassifierDBHandle dbHandle(
//     std::move(docRockHandle),
//     std::move(listRockHandle)
//   );
//   auto documents = dbHandle.listAllClassifierDocuments("coll1");
//   EXPECT_EQ(0, documents.size());
// }
