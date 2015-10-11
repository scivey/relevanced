#include <string>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "commandLineFlags.h"
#include "buildServerOptions.h"
#include "server/ThriftServerWrapper.h"
#include "server/simpleServerBuilders.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::server;

int main(int argc, char **argv) {
  google::SetUsageMessage("Usage");
  google::ParseCommandLineFlags(&argc, &argv, true);
  thread t1([]() {
    auto options = buildOptions();
    auto server = buildNormalThriftServer(options);
    LOG(INFO) << "listening on port: " << options->getThriftPort();
    server->serve();
  });
  t1.join();
}
