#pragma once
#include <string>
#include <tuple>
#include "libunicode/CodePointIterator.h"

namespace relevanced {
namespace tokenizer {

class DestructiveTokenIterator {
 protected:
  std::string &text_;
  libunicode::CodePointIterator::Iterator codePointIter_;
  libunicode::CodePointIterator::Iterator endIter_;
  char *inputBegin_ {nullptr};
  char *inputIter_ {nullptr};
  char *inputEnd_ {nullptr};

  char *outputIter_ {nullptr};
  char *outputEnd_ {nullptr};

 public:
  DestructiveTokenIterator(std::string &text);
  bool next(std::tuple<bool, size_t, size_t> &outTuple);
};

} // tokenizer
} // relevanced