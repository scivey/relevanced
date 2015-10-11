#pragma once
#include <string>
#include <memory>
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
  std::string dataDir_{"/var/lib/relevanced/data"};
  int thriftPort_{8097};
  std::chrono::milliseconds taskExpireTime_{60000};
  bool integrationTestMode_{false};
  int rocksdbThreads_{8};
  int centroidUpdateThreads_{4};
  int similarityScoreThreads_{4};
  int documentProcessingThreads_{4};

 public:
  RelevanceServerOptions();
  std::string getDataDir();
  void setDataDir(std::string dataDir);
  int getThriftPort();
  void setThriftPort(int port);
  bool getIntegrationTestMode();
  std::chrono::milliseconds getTaskExpireTime();
  void setIntegrationTestMode(bool mode);
  int getRocksDbThreadCount();
  void setRocksDbThreadCount(int n);
  int getDocumentProcessingThreadCount();
  void setDocumentProcessingThreadCount(int n);
  int getSimilarityScoreThreadCount();
  void setSimilarityScoreThreadCount(int n);
  int getCentroidUpdateThreadCount();
  void setCentroidUpdateThreadCount(int n);
};

} // server
} // relevanced