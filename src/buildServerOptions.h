#pragma once
#include <cstdlib>
#include <memory>
#include <string>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/DynamicConverter.h>
#include <folly/FileUtil.h>

#include "server/RelevanceServerOptions.h"
#include "commandLineFlags.h"

namespace relevanced {
namespace server {

std::shared_ptr<RelevanceServerOptions> buildOptions() {
  auto options = std::make_shared<RelevanceServerOptions>();
  if (FLAGS_config_file.size() > 0) {
    std::string confStr;
    if (!folly::readFile(FLAGS_config_file.c_str(), confStr)) {
      LOG(INFO) << "error reading config file!";
      exit(1);
    }

    // handle JSON parsing exceptions here
    folly::dynamic parsedConf = folly::parseJson(confStr);

    auto confItems = parsedConf.items();
    auto confDataDir = parsedConf.find("data_dir");
    if (confDataDir != confItems.end()) {
      options->setDataDir(folly::convertTo<std::string>(confDataDir->second));
    }
    auto confPort = parsedConf.find("port");
    if (confPort != confItems.end()) {
      options->setThriftPort(folly::convertTo<int>(confPort->second));
    }
    auto confRocksThreads = parsedConf.find("rocks_db_threads");
    if (confRocksThreads != confItems.end()) {
      options->setRocksDbThreadCount(folly::convertTo<int>(confRocksThreads->second));
    }
    auto confCentroidUpdateThreads = parsedConf.find("centroid_update_threads");
    if (confCentroidUpdateThreads != confItems.end()) {
      options->setCentroidUpdateThreadCount(folly::convertTo<int>(confCentroidUpdateThreads->second));
    }
    auto confProcessingThreads = parsedConf.find("document_processing_threads");
    if (confProcessingThreads != confItems.end()) {
      options->setDocumentProcessingThreadCount(folly::convertTo<int>(confProcessingThreads->second));
    }
    auto confScoringThreads = parsedConf.find("similarity_score_threads");
    if (confScoringThreads != confItems.end()) {
      options->setSimilarityScoreThreadCount(folly::convertTo<int>(confScoringThreads->second));
    }
  }

  if (FLAGS_data_dir.size() > 0) {
    options->setDataDir(FLAGS_data_dir);
  }
  if (FLAGS_port > 0) {
    options->setThriftPort(FLAGS_port);
  }
  if (FLAGS_rocks_db_threads > 0) {
    options->setRocksDbThreadCount(FLAGS_rocks_db_threads);
  }
  if (FLAGS_centroid_update_threads > 0) {
    options->setCentroidUpdateThreadCount(FLAGS_centroid_update_threads);
  }
  if (FLAGS_document_processing_threads > 0) {
    options->setDocumentProcessingThreadCount(FLAGS_document_processing_threads);
  }
  if (FLAGS_similarity_score_threads > 0) {
    options->setSimilarityScoreThreadCount(FLAGS_similarity_score_threads);
  }

  options->setIntegrationTestMode(FLAGS_integration_test_mode);
  return options;
}

} // server
} // relevanced
