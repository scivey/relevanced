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
  inputBegin_ = inputIter_;

  auto endPtr = ((uintptr_t) text.c_str()) + text.size();
  inputEnd_ = (char*) endPtr;
}

bool DestructiveTokenIterator::next(std::tuple<bool, size_t, size_t> &outTuple) {
  if (inputIter_ != inputEnd_) {
    size_t currentSize = 0;
    char *currentStartPointer = inputIter_;
    while (inputIter_ != inputEnd_) {
      auto codePoint = utf8::next(inputIter_, inputEnd_);
      auto block = libunicode::getUnicodeBlock(codePoint);
      codePoint = libunicode::normalizeCodePoint(codePoint, block);
      outputIter_ = utf8::append(codePoint, outputIter_);
      if (libunicode::isLetterPoint(codePoint, block)) {
        currentSize += 1;
      } else {
        if (currentSize > 2) {
          break;
        } else {
          currentSize = 0;
          currentStartPointer = inputIter_;
        }
      }
    }
    if (currentSize > 2) {

      // we have to do some gross pointer arithmetic here
      // because we can't directly know the length of the
      // byte range we're interested in based on the number
      // of code points we have. (they vary in size)

      // instead we calculate byte range based on current position
      // of the iterator (set by the utf8 library), the position
      // of that iterator at the start of this "good word cycle",
      // and the pointer to the base of the string.

      auto inputStart = (uintptr_t) inputBegin_;
      uintptr_t startChar = ((uintptr_t) currentStartPointer) - inputStart;
      uintptr_t endChar = ((uintptr_t) inputIter_) - inputStart;
      if (inputIter_ != inputEnd_) {
        endChar -= 1;
      }
      size_t startSt = (size_t) startChar;
      size_t endSt = (size_t) endChar;
      std::get<0>(outTuple) = true;
      std::get<1>(outTuple) = startSt;
      std::get<2>(outTuple) = endSt;
      return true;
    }
  }
  std::get<0>(outTuple) = false;
  std::get<1>(outTuple) = 0;
  std::get<2>(outTuple) = 0;
  return false;
}

} // tokenizer
} // relevanced