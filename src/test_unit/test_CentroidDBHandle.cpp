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
#include "persistence/CentroidDBHandle.h"
#include "persistence/RockHandle.h"
#include "ProcessedCentroid.h"

using namespace std;
using persistence::CentroidDBHandle;
using persistence::RockHandleIf;
using util::UniquePointer;
using ::testing::Return;
using ::testing::_;

#include "MockRock.h"

TEST(CentroidDBHandle, DoesCentroidExistTrue) {
  MockRock mockRock;
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  string centroidId {"centroid-id"};
  EXPECT_CALL(mockRock, exists(centroidId))
    .WillOnce(Return(true));
  CentroidDBHandle dbHandle(std::move(rockHandle));
  EXPECT_TRUE(dbHandle.doesCentroidExist(centroidId));
}

TEST(CentroidDBHandle, DoesCentroidExistFalse) {
  MockRock mockRock;
  UniquePointer<RockHandleIf> rockHandle(
    &mockRock, NonDeleter<RockHandleIf>()
  );
  string centroidId {"centroid-id"};
  EXPECT_CALL(mockRock, exists(centroidId))
    .WillOnce(Return(false));
  CentroidDBHandle dbHandle(std::move(rockHandle));
  EXPECT_FALSE(dbHandle.doesCentroidExist(centroidId));
}
