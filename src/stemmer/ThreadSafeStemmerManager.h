#pragma once

#include "stemmer/StemmerManagerIf.h"
#include "stemmer/StemmerIf.h"

#include <memory>
#include <map>
#include <folly/ThreadLocal.h>
#include "gen-cpp2/RelevancedProtocol_types.h"

namespace relevanced {
namespace stemmer {

class ThreadSafeStemmerManager: public StemmerManagerIf {
protected:
  folly::ThreadLocal<std::map<thrift_protocol::Language, std::shared_ptr<StemmerIf>>> stemmers_;
public:
  std::shared_ptr<StemmerIf> getStemmer(thrift_protocol::Language lang) override;
};

}
}

