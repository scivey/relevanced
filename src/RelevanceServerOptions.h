#pragma once
#include <string>

struct RelevanceServerOptions {
  const std::string dataDir;
  const int thriftPort;
  const std::chrono::milliseconds taskExpireTime{60000};
public:
  RelevanceServerOptions(std::string data, int port): dataDir(data), thriftPort(port){}
};
