#pragma once

// Adapted from MITIE: mitie/conll_tokenizer.h
// See https://github.com/mit-nlp/MITIE/blob/08d9bf9d8634e86001e2f5124342949ad456770b/mitielib/include/mitie/conll_tokenizer.h
// Original license:
//        Copyright (C) 2014 Massachusetts Institute of Technology, Lincoln Laboratory
//        License: Boost Software License   See LICENSE.txt for the full license.
//        Authors: Davis E. King (davis@dlib.net)

#include <tuple>
#include <cstring>
#include "text_util/ConllTokenIterator.h"

namespace relevanced {
namespace text_util {

class DestructiveConllTokenIterator: public ConllTokenIteratorIf {
protected:
  const char *text_;
  char *unsafeText_;
  size_t length_;
  size_t offset_ {0};
public:
  DestructiveConllTokenIterator(const char *text, size_t len): text_(text), length_(len) {
    unsafeText_ = (char*) text_;
  }
  bool next(std::tuple<bool, size_t, size_t> &outTuple) override {
    if (offset_ >= (length_ - 1)) {
      std::get<0>(outTuple) = false;
      std::get<1>(outTuple) = 0;
      std::get<2>(outTuple) = 0;
      return false;
    }
    auto startPos = offset_;
    for (;;) {
      if (startPos >= (length_ - 1)) {
        break;
      }
      const char *currentPtr = text_ + startPos;
      char current = *currentPtr;
      if (current == '\'') {
        break;
      }
      if (isAsciiAlphaNumeric(current)) {
        break;
      }
      if (isAsciiWhitespace(current) || isPunctuation(current)) {
        startPos++;
        continue;
      }
      if ((unsigned char) current == 0xE2 && (startPos < (length_ - 2))) {
        auto next1 = (unsigned char) *(currentPtr + 1);
        auto next2 = (unsigned char) *(currentPtr + 2);
        if (next1 == 0x80 && next2 == 0x99) {
          startPos += 3;
          continue;
        }
      }
      break;
    }
    auto endPos = startPos;
    char prevChar = *(text_ + startPos);
    for (size_t i = endPos; i < length_; i++) {
      const char *currentPtr = text_ + i;
      char current = *currentPtr;
      if (current == '\'' && i == startPos) {
        prevChar = current;
        continue;
      }
      if (current == '.' || current == ',') {
        size_t prevCharOffset = i - 1;
        if (prevCharOffset >= startPos && isAsciiNumeric(prevChar)) {
          prevChar = current;
          continue;
        } else {
          endPos = i;
          break;
        }
      }
      if (isAsciiAlphaNumeric(current)) {
        current = std::tolower(current);
        unsafeText_[i] = current;
        prevChar = current;
        continue;
      }
      if (isPunctuation(current) || isAsciiWhitespace(current)) {
        endPos = i;
        break;
      }
    }
    if (endPos > length_) {
      endPos = length_;
    }
    if (endPos == startPos) {
      endPos = length_;
    }
    offset_ = endPos;
    if ((endPos - startPos) == 0) {
      std::get<0>(outTuple) = false;
      std::get<1>(outTuple) = 0;
      std::get<2>(outTuple) = 0;
      return false;
    }
    std::get<0>(outTuple) = true;
    std::get<1>(outTuple) = startPos;
    std::get<2>(outTuple) = endPos;
    return true;
  }
};

} // text_util
} // relevanced
