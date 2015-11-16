#include <string>
#include "libunicode/CodePointIterator.h"
#include <utf8.h>

using namespace std;

namespace relevanced {
namespace libunicode {

CodePointIterator::Iterator::Iterator(char *it, char *end, bool isGood)
  : it_(it), end_(end), isGoodIter_(isGood) {

  if (isGoodIter_) {
    current_ = utf8::unchecked::next(it_);
  }
}
CodePointIterator::Iterator::Iterator(char *it, char *end)
  : CodePointIterator::Iterator::Iterator(it, end, false) {}

CodePointIterator::Iterator::Iterator()
  : it_(nullptr), end_(nullptr), isGoodIter_(false) {}

CodePointIterator::Iterator& CodePointIterator::Iterator::operator++() {

  // unchecked because utf8's error handling prevents
  // the iterator from ever pointing to the same place
  // as the 'end' iterator
  current_ = utf8::unchecked::next(it_);
  return *this;
}

CodePointIterator::Iterator CodePointIterator::Iterator::operator++(int) {
  auto temp = *this;
  ++*this;
  return temp;
}

bool CodePointIterator::Iterator::operator!=(const CodePointIterator::Iterator &other) {
  if (other.it_ == it_ && other.end_ == end_) {
    return false;
  }
  return true;
}

char* CodePointIterator::getEnd() {
  uintptr_t start = (uintptr_t) text_.c_str();
  start += text_.size();
  return (char*) start;
}
CodePointIterator::Iterator CodePointIterator::begin() {
  char *start = (char*) text_.c_str();
  char *end = getEnd();
  bool isGood = true;
  return CodePointIterator::Iterator(start, end, isGood);
}

CodePointIterator::Iterator CodePointIterator::end() {
  auto start = getEnd();
  auto end = getEnd();
  auto startPtr = (uintptr_t) start;
  startPtr++;
  start = (char*) startPtr;
  return CodePointIterator::Iterator(start, end);
}

} // text_util
} // relevanced
