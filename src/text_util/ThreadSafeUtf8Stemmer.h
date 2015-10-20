#pragma once
#include "text_util/Utf8Stemmer.h"
#include <folly/ThreadLocal.h>

namespace relevanced {
namespace text_util {

class ThreadSafeUtf8Stemmer {
protected:
  folly::ThreadLocal<Utf8Stemmer> stemmer_;
public:
  ThreadSafeUtf8Stemmer(){}
  size_t getStemPos(const char *toStem, size_t length) {
    return stemmer_->getStemPos(toStem, length);
  }

};

} // text_util
} // relevanced
