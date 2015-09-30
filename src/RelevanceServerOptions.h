#pragma once
#include <string>

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
