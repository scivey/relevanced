#pragma once
#include <glog/logging.h>
#include <folly/Format.h>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

using namespace std;
using namespace folly;

namespace relevanced {
namespace serialization {


template<typename T>
struct WriteBytes {
  static size_t write(unsigned char *data, T t) {
    size_t total = sizeof(decltype(t));
    unsigned char* elemPtr = (unsigned char*) &t;
    for (size_t i = 0; i < total; i++) {
      data[i] = elemPtr[i];
    }
    return total;
  }
};

template<>
struct WriteBytes<string> {
  static size_t write(unsigned char *data, string t) {
    size_t total = t.size();
    unsigned char *elemPtr = (unsigned char*) t.c_str();
    for (size_t i = 0; i < total; i++) {
      data[i] = elemPtr[i];
    }
    size_t remaining = total % 8;
    for (size_t i = total; i < (total + remaining); i++) {
      data[i] = '|';
    }
    return total + remaining;
  }
};

template<typename T>
size_t writeBytes(unsigned char *data, T t) {
  return WriteBytes<T>::write(data, t);
}

class ByteArrayWriter {
  unsigned char *data_ {nullptr};
  size_t index_ {0};
  size_t currentMax_;
public:
  ByteArrayWriter(size_t sizeHint): currentMax_(sizeHint) {
    data_ = (unsigned char*) malloc(sizeHint);
  }
  void expandIfNeeded(size_t count){
    if (((currentMax_ - index_) - 1) < count) {
      expand();
    }
  }
  unsigned char *getData () {
    return data_;
  }
  size_t getSize() {
    return index_;
  }

  size_t write(string &str) {
    size_t count = str.size() + 8;
    expandIfNeeded(count);
    unsigned char *sizeHead = data_ + index_;
    unsigned char *writeHead = sizeHead + sizeof(size_t);
    size_t written = writeBytes(writeHead, str);
    writeBytes(sizeHead, written);
    index_ += written + sizeof(size_t);
    return written;
  }

  template<typename T>
  size_t write(T t) {
    size_t count = sizeof(decltype(t));
    expandIfNeeded(count);
    unsigned char *sizeHead = data_ + index_;
    unsigned char *writeHead = sizeHead + sizeof(size_t);
    size_t written = writeBytes(writeHead, t);
    writeBytes(sizeHead, written);
    index_ += written + sizeof(size_t);
    return written;
  }
  void finalize() {
    expandIfNeeded(sizeof(size_t));
    size_t finished = 0;
    writeBytes(data_ + index_, finished);
    index_ += sizeof(size_t);
  }
  void expand() {
    size_t newMax = currentMax_ * 1.8;
    data_ = (unsigned char*) realloc((void*) data_, newMax);
    assert(data_ != nullptr);
    currentMax_ = newMax;
  }
};

class ByteReader {
  unsigned char *bytes_;
  size_t index_ {0};
public:
  ByteReader(unsigned char *bytes): bytes_(bytes) {}
  template<typename T>
  void read(T &result) {
    unsigned char *readSizeHead = bytes_ + index_;
    unsigned char *readDataHead = readSizeHead + sizeof(size_t);
    size_t count = sizeof(decltype(result));
    T* resPtr = (T*) (readDataHead);
    result = *resPtr;
    index_ += count;
    index_ += sizeof(size_t);
  }
  bool isFinished() {
    size_t *currentSize = (size_t*) (bytes_ + index_);
    return (*currentSize) == 0;
  }
  void readString(string &result) {
    unsigned char *readSizeHead = bytes_ + index_;
    unsigned char *readDataHead = readSizeHead + sizeof(size_t);
    size_t *charCount_ = (size_t*) readSizeHead;
    size_t charCount = *charCount_;
    for (size_t i = 0; i < charCount; i++) {
      result.push_back((char) readDataHead[i]);
    }
    index_ += charCount;
    index_ += sizeof(size_t);
  }
};

} // serialization
} // relevanced
