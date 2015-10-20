#pragma once

// Adapted from MITIE: mitie/conll_tokenizer.h
// See https://github.com/mit-nlp/MITIE/blob/08d9bf9d8634e86001e2f5124342949ad456770b/mitielib/include/mitie/conll_tokenizer.h
// Original license:
//        Copyright (C) 2014 Massachusetts Institute of Technology, Lincoln Laboratory
//        License: Boost Software License   See LICENSE.txt for the full license.
//        Authors: Davis E. King (davis@dlib.net)

#include <tuple>
#include <cstring>

namespace relevanced {
namespace text_util {

class ConllTokenIteratorIf {
public:
  virtual bool next(std::tuple<bool, size_t, size_t> &outTuple) = 0;
  bool isAsciiAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }
  bool isAsciiNumeric(char c) {
    return (c >= '0' && c <= '9');
  }
  bool isAsciiAlphaNumeric(char c) {
    return isAsciiAlpha(c) || isAsciiNumeric(c);
  }
  bool isAsciiWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
  }
  bool isPunctuation(char c) {
    return (c < '0' || (c < 'A' && c > '9') || (c > 'Z' && c < 'a') || c > 'z');
  }
};

class ConllTokenIterator: public ConllTokenIteratorIf {
protected:
  const char *text_;
  size_t length_;
  size_t offset_ {0};
public:
  ConllTokenIterator(const char *text, size_t len): text_(text), length_(len) {}
  bool next(std::tuple<bool, size_t, size_t> &outTuple) override;
};


} // text_util
} // relevanced
