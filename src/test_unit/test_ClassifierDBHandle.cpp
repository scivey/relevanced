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
#include "persistence/ClassifierDBHandle.h"
#include "persistence/RockHandle.h"
#include "persistence/InMemoryRockHandle.h"
#include "serialization/serializers.h"
#include "MockRock.h"

using namespace std;
using persistence::ClassifierDBHandle;
using persistence::RockHandleIf;
using persistence::InMemoryRockHandle;
using util::UniquePointer;
using ::testing::Return;
using ::testing::_;

TEST(ClassifierDBHandle, DoesClassifierExistTrue) {
  MockRock docMockRock;
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  MockRock listMockRock;
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string id {"classifier-id"};
  EXPECT_CALL(listMockRock, exists(id))
    .WillOnce(Return(true));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.doesClassifierExist(id));
}

TEST(ClassifierDBHandle, DoesClassifierExistFalse) {
  MockRock docMockRock;
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  MockRock listMockRock;
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string id {"classifier-id"};
  EXPECT_CALL(listMockRock, exists(id))
    .WillOnce(Return(false));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.doesClassifierExist(id));
}

TEST(ClassifierDBHandle, CreateClassifierExists) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string data {"value"};
  listMockRock.put("classifier", data);
  EXPECT_TRUE(listMockRock.exists("classifier"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_FALSE(dbHandle.createClassifier("classifier"));

  EXPECT_EQ("value", listMockRock.get("classifier"));
}

TEST(ClassifierDBHandle, CreateClassifierDoesNotExist) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(listMockRock.exists("classifier"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_TRUE(dbHandle.createClassifier("classifier"));

  EXPECT_TRUE(listMockRock.get("classifier") != "");
}

TEST(ClassifierDBHandle, DoesClassifierHaveDocumentTrue) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string val {"1"};
  docMockRock.put("classifier_id:document_id", val);
  EXPECT_TRUE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_TRUE(dbHandle.doesClassifierHaveDocument("classifier_id", "document_id"));
}

TEST(ClassifierDBHandle, DoesClassifierHaveDocumentFalse) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );

  EXPECT_FALSE(dbHandle.doesClassifierHaveDocument("classifier_id", "document_id"));
}

TEST(ClassifierDBHandle, AddPositiveDocumentToClassifierHappy) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("classifier_id", collVal);
  EXPECT_TRUE(listMockRock.exists("classifier_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.addPositiveDocumentToClassifier("classifier_id", "document_id"));
  EXPECT_EQ("1", docMockRock.get("classifier_id:document_id"));
}

TEST(ClassifierDBHandle, AddPositiveDocumentToClassifierMissingClassifier) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(listMockRock.exists("classifier_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addPositiveDocumentToClassifier("classifier_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
}

TEST(ClassifierDBHandle, AddPositiveDocumentToClassifierAlreadyExists) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("classifier_id", collVal);
  EXPECT_TRUE(listMockRock.exists("classifier_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  string existing {"existing_value"};
  docMockRock.put("classifier_id:document_id", existing);
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addPositiveDocumentToClassifier("classifier_id", "document_id"));
  EXPECT_EQ("existing_value", docMockRock.get("classifier_id:document_id"));
}

TEST(ClassifierDBHandle, AddNegativeDocumentToClassifierHappy) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("classifier_id", collVal);
  EXPECT_TRUE(listMockRock.exists("classifier_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.addNegativeDocumentToClassifier("classifier_id", "document_id"));
  EXPECT_EQ("0", docMockRock.get("classifier_id:document_id"));
}

TEST(ClassifierDBHandle, AddNegativeDocumentToClassifierMissingClassifier) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(listMockRock.exists("classifier_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addNegativeDocumentToClassifier("classifier_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
}

TEST(ClassifierDBHandle, AddNegativeDocumentToClassifierAlreadyExists) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string collVal {"1"};
  listMockRock.put("classifier_id", collVal);
  EXPECT_TRUE(listMockRock.exists("classifier_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  string existing {"existing_value"};
  docMockRock.put("classifier_id:document_id", existing);
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.addNegativeDocumentToClassifier("classifier_id", "document_id"));
  EXPECT_EQ("existing_value", docMockRock.get("classifier_id:document_id"));
}


TEST(ClassifierDBHandle, RemoveDocFromClassifierHappy) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  string keyVal {"anything"};
  docMockRock.put("classifier_id:document_id", keyVal);
  EXPECT_TRUE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_TRUE(dbHandle.removeDocumentFromClassifier("classifier_id", "document_id"));
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
}

TEST(ClassifierDBHandle, RemoveDocFromClassifierSadPanda) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  EXPECT_FALSE(docMockRock.exists("classifier_id:document_id"));
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  EXPECT_FALSE(dbHandle.removeDocumentFromClassifier("classifier_id", "document_id"));
}

TEST(ClassifierDBHandle, ListClassifiers) {
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
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto classifiers = dbHandle.listClassifiers();
  EXPECT_EQ(3, classifiers.size());
  EXPECT_EQ("coll1", classifiers.at(0));
  EXPECT_EQ("coll2", classifiers.at(1));
  EXPECT_EQ("coll3", classifiers.at(2));
}

TEST(ClassifierDBHandle, ListClassifiersEmpty) {
  InMemoryRockHandle docMockRock("doc");
  UniquePointer<RockHandleIf> docRockHandle(
    &docMockRock, NonDeleter<RockHandleIf>()
  );
  InMemoryRockHandle listMockRock("list");
  UniquePointer<RockHandleIf> listRockHandle(
    &listMockRock, NonDeleter<RockHandleIf>()
  );
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto classifiers = dbHandle.listClassifiers();
  EXPECT_EQ(0, classifiers.size());
}

TEST(ClassifierDBHandle, ListAllClassifierDocuments) {
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
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto documents = dbHandle.listAllClassifierDocuments("coll1");
  EXPECT_EQ(4, documents.size());
  EXPECT_EQ("doc1", documents.at(0));
  EXPECT_EQ("doc2", documents.at(1));
  EXPECT_EQ("doc3", documents.at(2));
  EXPECT_EQ("doc4", documents.at(3));
}

TEST(ClassifierDBHandle, ListAllClassifierDocumentsEmpty) {
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
  ClassifierDBHandle dbHandle(
    std::move(docRockHandle),
    std::move(listRockHandle)
  );
  auto documents = dbHandle.listAllClassifierDocuments("coll1");
  EXPECT_EQ(0, documents.size());
}