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

TEST(CollectionDBHandle, DoesCollectionHaveDocTrue) {
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

  EXPECT_TRUE(dbHandle.doesCollectionHaveDoc("collection_id", "document_id"));
}

TEST(CollectionDBHandle, DoesCollectionHaveDocFalse) {
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

  EXPECT_FALSE(dbHandle.doesCollectionHaveDoc("collection_id", "document_id"));
}

TEST(CollectionDBHandle, AddPositiveDocToCollectionHappy) {
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
  EXPECT_TRUE(dbHandle.addPositiveDocToCollection("collection_id", "document_id"));
  EXPECT_EQ("1", docMockRock.get("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddPositiveDocToCollectionMissingCollection) {
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
  EXPECT_FALSE(dbHandle.addPositiveDocToCollection("collection_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddPositiveDocToCollectionAlreadyExists) {
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
  EXPECT_FALSE(dbHandle.addPositiveDocToCollection("collection_id", "document_id"));
  EXPECT_EQ("existing_value", docMockRock.get("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddNegativeDocToCollectionHappy) {
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
  EXPECT_TRUE(dbHandle.addNegativeDocToCollection("collection_id", "document_id"));
  EXPECT_EQ("0", docMockRock.get("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddNegativeDocToCollectionMissingCollection) {
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
  EXPECT_FALSE(dbHandle.addNegativeDocToCollection("collection_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("collection_id:document_id"));
}

TEST(CollectionDBHandle, AddNegativeDocToCollectionAlreadyExists) {
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
  EXPECT_FALSE(dbHandle.addNegativeDocToCollection("collection_id", "document_id"));
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
  EXPECT_TRUE(dbHandle.removeDocFromCollection("collection_id", "document_id"));
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
  EXPECT_FALSE(dbHandle.removeDocFromCollection("collection_id", "document_id"));
}
