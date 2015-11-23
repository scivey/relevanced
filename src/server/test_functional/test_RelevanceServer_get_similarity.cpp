#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <memory>

#include <glog/logging.h>

#include <folly/ExceptionWrapper.h>
#include <folly/Memory.h>
#include <folly/futures/Try.h>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/Optional.h>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "server/test_functional/RelevanceServerTestCtx.h"

using namespace std;
using namespace wangle;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;
using namespace relevanced::util;
using namespace relevanced::text_util;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::document_processing_worker;
using namespace relevanced::similarity_score_worker;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::server;
using namespace relevanced::thrift_protocol;

using ::testing::Return;
using ::testing::_;

struct SimilarityTestCtx {
  RelevanceServerTestCtx *baseCtx;
  SimilarityTestCtx(RelevanceServerTestCtx *parent): baseCtx(parent){}
  void init() {
    auto centroid1 = std::make_shared<Centroid>(
      "centroid-1-id", unordered_map<string, double> {{"cat", 0.5}, {"dog", 0.5}}, 15.3
    );
    auto centroid2 = std::make_shared<Centroid>(
      "centroid-2-id", unordered_map<string, double> {{"fish", 0.33}, {"cat", 0.33}, {"wombat", 0.33}}, 10.7
    );
    vector<Future<Try<bool>>> saves;
    saves.push_back(baseCtx->persistence->saveCentroid("centroid-1-id", centroid1));
    saves.push_back(baseCtx->persistence->saveCentroid("centroid-2-id", centroid2));
    collect(saves).get();
    vector<Future<bool>> reloads;
    reloads.push_back(baseCtx->scoreWorker->reloadCentroid("centroid-1-id"));
    reloads.push_back(baseCtx->scoreWorker->reloadCentroid("centroid-2-id"));
    collect(reloads).get();
    auto text = folly::make_unique<string>("this is dog some text about dogs");
    auto id = folly::make_unique<string>("doc-1-id");
    baseCtx->server->createDocumentWithID(
      std::move(id), std::move(text), Language::EN
    ).get();
  }
};

TEST(RelevanceServer, TestGetTextSimilarityHappy) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto scoreResponse = ctx.server->getTextSimilarity(
    folly::make_unique<string>("centroid-1-id"),
    folly::make_unique<string>("This is some dog related text which is also about a cat."),
    Language::EN
  ).get();
  EXPECT_FALSE(scoreResponse.hasException());
  auto similarity = scoreResponse.value();
  EXPECT_TRUE(similarity > 0);
  EXPECT_TRUE(similarity < 1);
}

TEST(RelevanceServer, TestGetTextSimilarityMissingCentroid) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto scoreResponse = ctx.server->getTextSimilarity(
    folly::make_unique<string>("unrelated-centroid-id"),
    folly::make_unique<string>("This is some dog related text which is also about a cat."),
    Language::EN
  ).get();
  EXPECT_TRUE(scoreResponse.hasException<ECentroidDoesNotExist>());
}

TEST(RelevanceServer, TestMultiGetTextSimilarityHappy) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto scoreResponse = ctx.server->multiGetTextSimilarity(
    folly::make_unique<vector<string>>(
      vector<string> {"centroid-1-id", "centroid-2-id"}
    ),
    folly::make_unique<string>("This is some dog related text which is also about a cat."),
    Language::EN
  ).get();
  EXPECT_FALSE(scoreResponse.hasException());
}


TEST(RelevanceServer, TestGetDocumentSimilarityHappy) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto result1 = ctx.server->getDocumentSimilarity(
    folly::make_unique<string>("centroid-1-id"),
    folly::make_unique<string>("doc-1-id")
  ).get();
  EXPECT_FALSE(result1.hasException());
  auto result2 = ctx.server->getDocumentSimilarity(
    folly::make_unique<string>("centroid-2-id"),
    folly::make_unique<string>("doc-1-id")
  ).get();
  EXPECT_FALSE(result2.hasException());
  EXPECT_TRUE(result1.value() > result2.value());
}

TEST(RelevanceServer, TestGetDocumentSimilarityMissingDocument) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto result = ctx.server->getDocumentSimilarity(
    folly::make_unique<string>("centroid-1-id"),
    folly::make_unique<string>("bad-doc-id")
  ).get();
  EXPECT_TRUE(result.hasException<EDocumentDoesNotExist>());
}

TEST(RelevanceServer, TestGetDocumentSimilarityMissingCentroid) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto result = ctx.server->getDocumentSimilarity(
    folly::make_unique<string>("bad-centroid-id"),
    folly::make_unique<string>("doc-1-id")
  ).get();
  EXPECT_TRUE(result.hasException<ECentroidDoesNotExist>());
}

TEST(RelevanceServer, TestMultiGetDocumentSimilarityHappy) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto result = ctx.server->multiGetDocumentSimilarity(
    folly::make_unique<vector<string>>(
      vector<string> {"centroid-1-id", "centroid-2-id"}
    ),
    folly::make_unique<string>("doc-1-id")
  ).get();
  EXPECT_FALSE(result.hasException());
}

TEST(RelevanceServer, TestMultiGetDocumentSimilarityMissingDocument) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto result = ctx.server->multiGetDocumentSimilarity(
    folly::make_unique<vector<string>>(
      vector<string> {"centroid-1-id", "centroid-2-id"}
    ),
    folly::make_unique<string>("bad-doc-id")
  ).get();
  EXPECT_TRUE(result.hasException<EDocumentDoesNotExist>());
}

TEST(RelevanceServer, TestMultiGetDocumentSimilarityMissingCentroid) {
  RelevanceServerTestCtx ctx;
  SimilarityTestCtx testCtx(&ctx);
  testCtx.init();
  auto result = ctx.server->multiGetDocumentSimilarity(
    folly::make_unique<vector<string>>(
      vector<string> {"centroid-1-id", "bad-centroid-id"}
    ),
    folly::make_unique<string>("doc-1-id")
  ).get();
  EXPECT_TRUE(result.hasException<ECentroidDoesNotExist>());
}