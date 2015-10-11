#pragma once
#include <cstdlib>
#include <memory>
#include <string>
#include <map>

#include <folly/json.h>
#include <folly/dynamic.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>
#include <folly/FileUtil.h>
#include <folly/MapUtil.h>
#include "server/RelevanceServerOptions.h"
#include "commandLineFlags.h"


namespace relevanced {
namespace server {

std::map<std::string, std::string> getEnvSettings() {
  std::map<std::string, std::string> envVarMap{
      {"RELEVANCED_DATA_DIR", "data_dir"},
      {"RELEVANCED_PORT", "port"},
      {"RELEVANCED_CONFIG_FILE", "config_file"},
      {"RELEVANCED_ROCKSDB_THREADS", "rocks_db_threads"},
      {"RELEVANCED_DOCUMENT_PROCESSING_THREADS", "document_processing_threads"},
      {"RELEVANCED_SIMILARITY_SCORE_THREADS", "similarity_score_threads"},
      {"RELEVANCED_CENTROID_UPDATE_THREADS", "centroid_update_threads"}};
  std::map<std::string, std::string> output;
  for (auto &elem : envVarMap) {
    char *charVal = getenv(elem.first.c_str());
    if (charVal != nullptr) {
      std::string value = charVal;
      output.insert(make_pair(elem.second, value));
    }
  }
  return output;
}

// this is a little gross, but it's also a straightforward and
// obvious way to do it. (and it's only run once on startup)

std::shared_ptr<RelevanceServerOptions> buildOptions() {
  auto options = std::make_shared<RelevanceServerOptions>();
  auto envSettings = getEnvSettings();

  std::string configFilePath = FLAGS_config_file;
  if (configFilePath.size() == 0) {
    auto confPathOption = folly::get_optional(envSettings, "config_file");
    if (confPathOption.hasValue()) {
      configFilePath = confPathOption.value();
    }
  }

  if (configFilePath.size() > 0) {
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
      options->setRocksDbThreadCount(
          folly::convertTo<int>(confRocksThreads->second));
    }
    auto confCentroidUpdateThreads = parsedConf.find("centroid_update_threads");
    if (confCentroidUpdateThreads != confItems.end()) {
      options->setCentroidUpdateThreadCount(
          folly::convertTo<int>(confCentroidUpdateThreads->second));
    }
    auto confProcessingThreads = parsedConf.find("document_processing_threads");
    if (confProcessingThreads != confItems.end()) {
      options->setDocumentProcessingThreadCount(
          folly::convertTo<int>(confProcessingThreads->second));
    }
    auto confScoringThreads = parsedConf.find("similarity_score_threads");
    if (confScoringThreads != confItems.end()) {
      options->setSimilarityScoreThreadCount(
          folly::convertTo<int>(confScoringThreads->second));
    }
  }

  {
    // add any settings from the environment variables
    auto envDataDir = folly::get_optional(envSettings, "data_dir");
    if (envDataDir.hasValue()) {
      options->setDataDir(envDataDir.value());
    }
    auto envPort = folly::get_optional(envSettings, "port");
    if (envPort.hasValue()) {
      options->setThriftPort(folly::to<int>(envPort.value()));
    }
    auto envRocksThreads = folly::get_optional(envSettings, "rocks_db_threads");
    if (envRocksThreads.hasValue()) {
      options->setRocksDbThreadCount(folly::to<int>(envRocksThreads.value()));
    }
    auto envUpdatingThreads =
        folly::get_optional(envSettings, "centroid_update_threads");
    if (envUpdatingThreads.hasValue()) {
      options->setCentroidUpdateThreadCount(
          folly::to<int>(envUpdatingThreads.value()));
    }
    auto envProcessingThreads =
        folly::get_optional(envSettings, "document_processing_threads");
    if (envProcessingThreads.hasValue()) {
      options->setDocumentProcessingThreadCount(
          folly::to<int>(envProcessingThreads.value()));
    }
    auto envScoringThreads =
        folly::get_optional(envSettings, "similarity_score_threads");
    if (envScoringThreads.hasValue()) {
      options->setSimilarityScoreThreadCount(
          folly::to<int>(envScoringThreads.value()));
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
    options->setDocumentProcessingThreadCount(
        FLAGS_document_processing_threads);
  }
  if (FLAGS_similarity_score_threads > 0) {
    options->setSimilarityScoreThreadCount(FLAGS_similarity_score_threads);
  }

  options->setIntegrationTestMode(FLAGS_integration_test_mode);
  return options;
}

} // server
} // relevanced
