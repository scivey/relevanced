#pragma once
#include "stemmer/StemmerIf.h"
#include "stemmer/Utf8Stemmer.h"

#include <folly/ThreadLocal.h>

namespace relevanced {
namespace stemmer {

class ThreadSafeUtf8Stemmer: public StemmerIf {
protected:
  folly::ThreadLocal<Utf8Stemmer> stemmer_;
public:
  ThreadSafeUtf8Stemmer(){}
  size_t getStemPos(const char *toStem, size_t length) override {
    return stemmer_->getStemPos(toStem, length);
  }

};

} // text_util
} // relevanced
