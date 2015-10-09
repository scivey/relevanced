#pragma once
#include <string>
#include <chrono>
#include <glog/logging.h>

namespace relevanced {
namespace server {

/**
 * Various configuration settings which can be modified with command
 * line switches.  This is passed into `ServerBuilder`.  In addition to
 * passing it a `shared_ptr` for these options into the `RelevanceServer`
 * it builds, `ServerBuilder` uses some of these options during build
 * and initialization.  The thread count settings are particuarly notable
 * there.
 */
class RelevanceServerOptions {
  std::string dataDir_ {"/var/lib/relevanced/data"};
  int thriftPort_ {8097};
  std::chrono::milliseconds taskExpireTime_{60000};
  bool integrationTestMode_ {false};
  int rocksdbThreads_ {8};
  int centroidUpdateThreads_ {4};
  int similarityScoreThreads_ {4};
  int documentProcessingThreads_ {4};

public:
  RelevanceServerOptions(){}

  std::string getDataDir() {
    LOG(INFO) << "getDataDir() -> " << dataDir_;
    return dataDir_;
  }

  void setDataDir(std::string dataDir) {
    dataDir_ = dataDir;
  }

  int getThriftPort() {
    return thriftPort_;
  }

  void setThriftPort(int port) {
    thriftPort_ = port;
  }

  bool getIntegrationTestMode() {
    return integrationTestMode_;
  }

  std::chrono::milliseconds getTaskExpireTime() {
    return taskExpireTime_;
  }

  void setIntegrationTestMode(bool mode) {
    integrationTestMode_ = mode;
  }

  int getRocksDbThreadCount() {
    return rocksdbThreads_;
  }

  void setRocksDbThreadCount(int n) {
    rocksdbThreads_ = n;
  }

  int getDocumentProcessingThreadCount() {
    return documentProcessingThreads_;
  }

  void setDocumentProcessingThreadCount(int n) {
    documentProcessingThreads_ = n;
  }

  int getSimilarityScoreThreadCount() {
    return similarityScoreThreads_;
  }

  void setSimilarityScoreThreadCount(int n) {
    similarityScoreThreads_ = n;
  }

  int getCentroidUpdateThreadCount() {
    return centroidUpdateThreads_;
  }

  void setCentroidUpdateThreadCount(int n) {
    centroidUpdateThreads_ = n;
  }
};

} // server
} // relevanced