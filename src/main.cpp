#include <string>
#include <memory>
#include <glog/logging.h>
#include "RelevanceCollectionManager.h"
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "stemmer/StemmerIf.h"
#include "stemmer/PorterStemmer.h"
#include "DocumentProcessor.h"
#include "RelevanceServer.h"
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/AsyncProcessor.h>

using stemmer::StemmerIf;
using stemmer::PorterStemmer;
using stopwords::StopwordFilter;
using stopwords::StopwordFilterIf;
using tokenizer::TokenizerIf;
using tokenizer::Tokenizer;
using namespace std;
using namespace folly;

int main() {
  LOG(INFO) << "start";
  thread t1([](){
    shared_ptr<TokenizerIf> tokenizer(
      (TokenizerIf*) new Tokenizer
    );
    shared_ptr<StemmerIf> stemmer(
      (StemmerIf*) new PorterStemmer
    );
    shared_ptr<StopwordFilterIf> stopwordFilter(
      (StopwordFilterIf*) new StopwordFilter
    );
    auto documentProcessor = make_shared<DocumentProcessor>(
      stemmer,
      tokenizer,
      stopwordFilter
    );
    auto manager = new RelevanceCollectionManager(documentProcessor);
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