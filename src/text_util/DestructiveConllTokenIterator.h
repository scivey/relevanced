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
  bool next(std::tuple<bool, size_t, size_t> &outTuple) override;
};

} // text_util
} // relevanced
