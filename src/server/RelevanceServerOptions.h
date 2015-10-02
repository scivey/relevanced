#pragma once
#include <string>

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
struct RelevanceServerOptions {
  const std::string dataDir;
  const int thriftPort;
  const std::chrono::milliseconds taskExpireTime{60000};
  int rocksdbThreads_ {8};
  int centroidUpdateThreads_ {4};
  int similarityScoreThreads_ {4};
  int documentProcessingThreads_ {4};

public:
  RelevanceServerOptions(std::string data, int port): dataDir(data), thriftPort(port){}
  void setRocksDbThreadCount(int n) {
    rocksdbThreads_ = n;
  }
  int getRocksDbThreadCount() {
    return rocksdbThreads_;
  }
  void setDocumentProcessingThreadCount(int n) {
    documentProcessingThreads_ = n;
  }
  int getDocumentProcessingThreadCount() {
    return documentProcessingThreads_;
  }
  void setSimilarityScoreThreadCount(int n) {
    rocksdbThreads_ = n;
  }
  int getSimilarityScoreThreadCount() {
    return rocksdbThreads_;
  }
  void setCentroidUpdateThreadCount(int n) {
    centroidUpdateThreads_ = n;
  }
  int getCentroidUpdateThreadCount() {
    return centroidUpdateThreads_;
  }
};

} // server
} // relevanced