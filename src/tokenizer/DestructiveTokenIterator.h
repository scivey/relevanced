#pragma once
#include <string>
#include <tuple>

namespace relevanced {
namespace tokenizer {

class DestructiveTokenIterator {
 protected:
  std::string &text_;
  char *inputBegin_ {nullptr};
  char *inputIter_ {nullptr};
  char *inputEnd_ {nullptr};

  char *outputIter_ {nullptr};

 public:
  DestructiveTokenIterator(std::string &text);
  bool next(std::tuple<bool, size_t, size_t> &outTuple);
};

} // tokenizer
} // relevanced