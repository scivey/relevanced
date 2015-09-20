#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

#include <glog/logging.h>
#include <folly/Format.h>
#include <folly/Format.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <eigen3/Eigen/Dense>
#include "data.h"
#include "WhitespaceTokenizer.h"
#include "stopwords.h"
#include "stringUtil.h"
#include "util.h"
#include "Centroid.h"
#include "Tfidf.h"
#include "CentroidFactory.h"
#include "RelevanceCollectionManager.h"
#include "RelevanceServer.h"
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

using tokenizer::WhitespaceTokenizer;
using namespace std;
using namespace folly;
using namespace Eigen;

int main() {
  LOG(INFO) << "start";
  thread t1([](){
    auto manager = new RelevanceCollectionManager;
    auto service = make_shared<RelevanceServer>(manager);
    bool allowInsecureLoopback = true;
    string saslPolicy = "";
    auto server = new apache::thrift::ThriftServer(
      saslPolicy, allowInsecureLoopback
    );
    server->setInterface(service);
    auto port = 8097;
    server->setPort(port);
    LOG(INFO) << "listening on: " << port;
    server->serve();
  });
  t1.join();
  LOG(INFO) << "end";
}