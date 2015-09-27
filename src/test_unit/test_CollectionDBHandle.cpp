#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include "util.h"
#include "TestHelpers.h"
#include "ProcessedDocument.h"
#include "persistence/CollectionDBHandle.h"
#include "persistence/RockHandle.h"
#include "persistence/InMemoryRockHandle.h"
#include "serialization/serializers.h"
#include "MockRock.h"

using namespace std;
using persistence::CollectionDBHandle;
using persistence::RockHandleIf;
using persistence::InMemoryRockHandle;
using util::UniquePointer;
using ::testing::Return;
using ::testing::_;

TEST(CollectionDBHandle, DoesCollectionExistTrue) {
  MockRock docMockRock;
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  MockRock listMockRock;
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string id {"collection-id"};
  EXPECT_CALL(listMockRock, exists(id))
    .WillOnce(Return(true));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.doesCollectionExist(id));
}

TEST(CollectionDBHandle, DoesCollectionExistFalse) {
  MockRock docMockRock;
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  MockRock listMockRock;
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string id {"collection-id"};
  EXPECT_CALL(listMockRock, exists(id))
    .WillOnce(Return(false));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.doesCollectionExist(id));
}

TEST(CollectionDBHandle, CreateCollectionExists) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string data {"value"};
  listMockRock.put("collection", data);
  EXPECT_TRUE(listMockRock.exists("collection"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_FALSE(dbHandle.createCollection("collection"));

  EXPECT_EQ("value", listMockRock.get("collection"));
}

TEST(CollectionDBHandle, CreateCollectionDoesNotExist) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(listMockRock.exists("collection"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_TRUE(dbHandle.createCollection("collection"));

  EXPECT_TRUE(listMockRock.get("collection") != "");
}

TEST(CollectionDBHandle, DoesCollectionHaveDocumentTrue) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string val {"1"};
  docMockRock.put("collection_id:document_id", val);
  EXPECT_TRUE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_TRUE(dbHandle.doesCollectionHaveDocument("collection_id", "document_id"));
}

TEST(CollectionDBHandle, DoesCollectionHaveDocumentFalse) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_FALSE(dbHandle.doesCollectionHaveDocument("collection_id", "document_id"));
}

TEST(CollectionDBHandle, AddPositiveDocumentToCollectionHappy) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("collection_id", collVal);
  EXPECT_TRUE(listMockRock.exists("collection_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.addPositiveDocumentToCollection("collection_id", "document_id"));
  EXPECT_EQ("1", docMockRock.get("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddPositiveDocumentToCollectionMissingCollection) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(listMockRock.exists("collection_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addPositiveDocumentToCollection("collection_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddPositiveDocumentToCollectionAlreadyExists) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("collection_id", collVal);
  EXPECT_TRUE(listMockRock.exists("collection_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  string existing {"existing_value"};
  docMockRock.put("collection_id:document_id", existing);
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addPositiveDocumentToCollection("collection_id", "document_id"));
  EXPECT_EQ("existing_value", docMockRock.get("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddNegativeDocumentToCollectionHappy) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("collection_id", collVal);
  EXPECT_TRUE(listMockRock.exists("collection_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.addNegativeDocumentToCollection("collection_id", "document_id"));
  EXPECT_EQ("0", docMockRock.get("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddNegativeDocumentToCollectionMissingCollection) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(listMockRock.exists("collection_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addNegativeDocumentToCollection("collection_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddNegativeDocumentToCollectionAlreadyExists) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("collection_id", collVal);
  EXPECT_TRUE(listMockRock.exists("collection_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  string existing {"existing_value"};
  docMockRock.put("collection_id:document_id", existing);
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addNegativeDocumentToCollection("collection_id", "document_id"));
  EXPECT_EQ("existing_value", docMockRock.get("collection_id:document_id"));
}


TEST(CollectionDBHandle, RemoveDocFromCollectionHappy) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string keyVal {"anything"};
  docMockRock.put("collection_id:document_id", keyVal);
  EXPECT_TRUE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.removeDocumentFromCollection("collection_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
}

TEST(CollectionDBHandle, RemoveDocFromCollectionSadPanda) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.removeDocumentFromCollection("collection_id", "document_id"));
}

TEST(CollectionDBHandle, ListCollections) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string anything {"anything"};
  listMockRock.put("coll1", anything);
  listMockRock.put("coll2", anything);
  listMockRock.put("coll3", anything);
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto collections = dbHandle.listCollections();
  EXPECT_EQ(3, collections.size());
  EXPECT_EQ("coll1", collections.at(0));
  EXPECT_EQ("coll2", collections.at(1));
  EXPECT_EQ("coll3", collections.at(2));
}

TEST(CollectionDBHandle, ListCollectionsEmpty) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto collections = dbHandle.listCollections();
  EXPECT_EQ(0, collections.size());
}

TEST(CollectionDBHandle, ListCollectionDocuments) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string anything {"1"};
  listMockRock.put("coll1", anything);
  docMockRock.put("coll1:doc1", anything);
  docMockRock.put("coll1:doc2", anything);
  docMockRock.put("coll1:doc3", anything);
  docMockRock.put("coll1:doc4", anything);
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto documents = dbHandle.listCollectionDocuments("coll1");
  EXPECT_EQ(4, documents.size());
  EXPECT_EQ("doc1", documents.at(0));
  EXPECT_EQ("doc2", documents.at(1));
  EXPECT_EQ("doc3", documents.at(2));
  EXPECT_EQ("doc4", documents.at(3));
}

TEST(CollectionDBHandle, ListCollectionDocumentsEmpty) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string anything {"1"};
  listMockRock.put("coll1", anything);
  CollectionDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto documents = dbHandle.listCollectionDocuments("coll1");
  EXPECT_EQ(0, documents.size());
}