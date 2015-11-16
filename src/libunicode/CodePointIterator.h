#pragma once
#include <string>

namespace relevanced {
namespace libunicode {

class CodePointIterator {
protected:
  const std::string &text_;
  char* getEnd();
public:

  class Iterator {
  protected:
    char *it_;
    char *end_;
    uint32_t current_ = 0;
    bool isGoodIter_ {false};
  public:
    Iterator();
    Iterator(char *it, char *end, bool isGood);
    Iterator(char *it, char *end);
    bool operator!=(const Iterator &other);
    uint32_t operator*() {
      return current_;
    }
    Iterator& operator++();
    Iterator operator++(int);
  };

  CodePointIterator(const std::string &text): text_(text){}
  Iterator begin();
  Iterator end();
};

} // text_util
} // relevanced