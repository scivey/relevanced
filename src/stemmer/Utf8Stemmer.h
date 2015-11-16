#pragma once

#include "stemmer/StemmerIf.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
struct sb_stemmer;

namespace relevanced {
namespace stemmer {

class Utf8Stemmer: public StemmerIf {
protected:
  struct sb_stemmer *stemmer_;
  thrift_protocol::Language language_;
public:
  Utf8Stemmer(thrift_protocol::Language lang);
  size_t getStemPos(const char *toStem, size_t length) override;
  ~Utf8Stemmer();
};

} // text_util
} // relevanced
