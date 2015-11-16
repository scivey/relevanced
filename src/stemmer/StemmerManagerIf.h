#pragma once
#include <memory>
#include "stemmer/StemmerIf.h"
#include "gen-cpp2/RelevancedProtocol_types.h"

namespace relevanced {
namespace stemmer {

class StemmerManagerIf {
public:
  virtual std::shared_ptr<StemmerIf> getStemmer(thrift_protocol::Language) = 0;
  virtual ~StemmerManagerIf() = default;
};

} // stemmer
} // relevanced

