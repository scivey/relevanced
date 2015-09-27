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
#include "persistence/DocumentDBHandle.h"
#include "persistence/RockHandle.h"
#include "persistence/InMemoryRockHandle.h"
#include "serialization/serializers.h"
#include "MockRock.h"

using namespace std;
using persistence::DocumentDBHandle;
using persistence::RockHandleIf;
using persistence::InMemoryRockHandle;
using util::UniquePointer;
using ::testing::Return;
using ::testing::_;

ProcessedDocument getProcessedDocument(string id) {
  map<string, double> wordCounts = {
    {"fish", 1.82},
    {"dog", 8.7}
  };
  ProcessedDocument doc(id, wordCounts);
  return doc;
}

TEST(DocumentDBHandle, DoesDocumentExistTrue) {
  MockRock mockRock;
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  string docId {"doc-id"};
  EXPECT_CALL(mockRock, exists(docId))
    .WillOnce(Return(true));
  DocumentDBHandle dbHandle(std::move(rockHandle));
  EXPECT_TRUE(dbHandle.doesDocumentExist(docId));
}

TEST(DocumentDBHandle, DoesDocumentExistFalse) {
  MockRock mockRock;
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  string docId {"doc-id"};
  EXPECT_CALL(mockRock, exists(docId))
    .WillOnce(Return(false));
  DocumentDBHandle dbHandle(std::move(rockHandle));
  EXPECT_FALSE(dbHandle.doesDocumentExist(docId));
}

TEST(DocumentDBHandle, SaveDocument1) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));

  string docId("doc-id");
  auto doc = getProcessedDocument(docId);
  EXPECT_FALSE(mockRock.exists("doc-id"));
  dbHandle.saveDocument(&doc);
  EXPECT_TRUE(mockRock.exists("doc-id"));
}

TEST(DocumentDBHandle, DeleteDocumentExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));
  string data = "something";
  mockRock.put("doc-id", data);
  EXPECT_TRUE(mockRock.exists("doc-id"));
  EXPECT_TRUE(dbHandle.deleteDocument("doc-id"));
  EXPECT_FALSE(mockRock.exists("doc-id"));
}

TEST(DocumentDBHandle, DeleteDocumentDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));
  EXPECT_FALSE(mockRock.exists("doc-id"));
  EXPECT_FALSE(dbHandle.deleteDocument("doc-id"));
  EXPECT_FALSE(mockRock.exists("doc-id"));
}

TEST(DocumentDBHandle, LoadDocumentSharedExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));
  map<string, double> wordCounts = {
    {"fish", 1.82},
    {"dog", 8.7}
  };
  ProcessedDocument doc("doc-id", wordCounts);

  unsigned char *serialized;
  size_t len = serialization::binarySerialize(&serialized, doc);
  rocksdb::Slice data((char*) serialized, len);
  EXPECT_FALSE(mockRock.exists("doc-id"));
  mockRock.put("doc-id", data);
  EXPECT_TRUE(mockRock.exists("doc-id"));

  auto result = dbHandle.loadDocumentShared("doc-id");

  EXPECT_TRUE(result.hasValue());
  auto loaded = result.value();
  EXPECT_EQ("doc-id", loaded->id);
  EXPECT_EQ(2, loaded->normalizedWordCounts.size());
  free(serialized);
}

TEST(DocumentDBHandle, LoadDocumentSharedDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));
  EXPECT_FALSE(mockRock.exists("doc-id"));

  auto result = dbHandle.loadDocumentShared("doc-id");

  EXPECT_FALSE(result.hasValue());
}

TEST(DocumentDBHandle, LoadDocumentExists) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));
  map<string, double> wordCounts = {
    {"fish", 1.82},
    {"dog", 8.7}
  };
  ProcessedDocument doc("doc-id", wordCounts);

  unsigned char *serialized;
  size_t len = serialization::binarySerialize(&serialized, doc);
  rocksdb::Slice data((char*) serialized, len);
  EXPECT_FALSE(mockRock.exists("doc-id"));
  mockRock.put("doc-id", data);
  EXPECT_TRUE(mockRock.exists("doc-id"));

  auto result = dbHandle.loadDocument("doc-id");

  EXPECT_TRUE(result.hasValue());
  auto loaded = std::move(result.value());
  EXPECT_EQ("doc-id", loaded->id);
  EXPECT_EQ(2, loaded->normalizedWordCounts.size());
  free(serialized);
}

TEST(DocumentDBHandle, LoadDocumentDoesNotExist) {
  InMemoryRockHandle mockRock("/some-path");
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  DocumentDBHandle dbHandle(std::move(rockHandle));
  EXPECT_FALSE(mockRock.exists("doc-id"));

  auto result = dbHandle.loadDocument("doc-id");

  EXPECT_FALSE(result.hasValue());
}
