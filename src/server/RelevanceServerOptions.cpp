#include <string>
#include <memory>
#include <chrono>
#include <glog/logging.h>
#include "server/RelevanceServerOptions.h"

using namespace std;

namespace relevanced {
namespace server {


RelevanceServerOptions::RelevanceServerOptions()
    : thriftPort_(8097),
      taskExpireTime_(60000),
      integrationTestMode_(false),
      rocksdbThreads_(8),
      centroidUpdateThreads_(4),
      similarityScoreThreads_(4),
      documentProcessingThreads_(4) {}

string RelevanceServerOptions::getDataDir() {
  LOG(INFO) << "getDataDir() -> " << dataDir_;
  return dataDir_;
}

void RelevanceServerOptions::setDataDir(std::string dataDir) {
  dataDir_ = dataDir;
}

int RelevanceServerOptions::getThriftPort() { return thriftPort_; }

void RelevanceServerOptions::setThriftPort(int port) { thriftPort_ = port; }

bool RelevanceServerOptions::getIntegrationTestMode() {
  return integrationTestMode_;
}

chrono::milliseconds RelevanceServerOptions::getTaskExpireTime() {
  return taskExpireTime_;
}

void RelevanceServerOptions::setIntegrationTestMode(bool mode) {
  integrationTestMode_ = mode;
}

int RelevanceServerOptions::getRocksDbThreadCount() { return rocksdbThreads_; }

void RelevanceServerOptions::setRocksDbThreadCount(int n) {
  rocksdbThreads_ = n;
}

int RelevanceServerOptions::getDocumentProcessingThreadCount() {
  return documentProcessingThreads_;
}

void RelevanceServerOptions::setDocumentProcessingThreadCount(int n) {
  documentProcessingThreads_ = n;
}

int RelevanceServerOptions::getSimilarityScoreThreadCount() {
  return similarityScoreThreads_;
}

void RelevanceServerOptions::setSimilarityScoreThreadCount(int n) {
  similarityScoreThreads_ = n;
}

int RelevanceServerOptions::getCentroidUpdateThreadCount() {
  return centroidUpdateThreads_;
}

void RelevanceServerOptions::setCentroidUpdateThreadCount(int n) {
  centroidUpdateThreads_ = n;
}

} // server
} // relevanced