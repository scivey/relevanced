#include "tokenizer/DestructiveTokenIterator.h"
#include <tuple>
#include <string>
#include <utf8.h>
#include "libunicode/code_point_support.h"
#include "libunicode/UnicodeBlock.h"

using namespace std;

namespace relevanced {
namespace tokenizer {

DestructiveTokenIterator::DestructiveTokenIterator(string &text)
  : text_(text) {
  outputIter_ = (char*) text.c_str();
  inputIter_ = (char*) text.c_str();
  inputBegin_ = inputIter;

  auto endPtr = ((uintptr_t) text.c_str()) + text.size();
  inputEnd_ = (char*) endPtr;
  outputEnd_ = (char*) endPtr;
}

bool DestructiveTokenIterator::next(std::tuple<bool, size_t, size_t> &outTuple) {
  if (inputIter_ == inputEnd_) {
    std::get<0>(outTuple) = false;
    std::get<1>(outTuple) = 0;
    std::get<2>(outTuple) = 0;
    return false;
  }
  size_t currentSize = 0;
  char *currentStartPointer = inputIter_;
  while (inputIter_ != inputEnd_) {
    auto codePoint = utf8::next(inputEnd_, inputIter_);
    auto block = libunicode::getUnicodeBlock(codePoint);
    codePoint = libunicode::normalizeCodePoint(codePoint, block);
    outputIter_ = utf8::append(codePoint, outputIter_);
    if (libunicode::isLetterPoint(codePoint)) {
      currentSize += 1;
    } else {
      if (currentSize > 2) {
        auto inputStart = (uintptr_t) inputBegin_;
        uintptr_t startChar = ((uintptr_t) currentStartPointer) - inputStart;
        uintptr_t endChar = ((uintptr_t) inputIter_) - inputStart;
        size_t startSt = (size_t) startChar;
        size_t endSt = (size_t) endChar;
        std::get<0>(outTuple) = true;
        std::get<1>(outTuple) = startSt;
        std::get<2>(outTuple) = endSt;
        return true;
      } else {
        currentSize = 0;
        currentStartPointer = inputIter_;
      }
    }
  }
  std::get<0>(outTuple) = false;
  std::get<1>(outTuple) = 0;
  std::get<2>(outTuple) = 0;
  return false;
}

class DestructiveTokenIterator {
 protected:
  std::string &text_;
 public:
  DestructiveTokenIterator(std::string &text): text_(text){}
  bool next(std::tuple<bool, size_t, size_t> &outTuple);
};

} // tokenizer
} // relevanced