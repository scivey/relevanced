#pragma once
#include <cstdlib>
#include "bytes.h"
#include <glog/logging.h>
namespace serialization {

template<typename T>
struct BinaryDeserializer {
  static void deserialize(unsigned char *bytes, T &result) {
    LOG(INFO) << "default deserialization!";
  }
};

template<typename T>
struct BinarySerializer {
  static size_t serialize(unsigned char **result, T &target) {
    unsigned char *res = (unsigned char*) malloc(1);
    *result = res;
    LOG(INFO) << "default serialization!";
    return 1;
  }
};

template<typename T>
size_t binarySerialize(unsigned char **result, T &target) {
  return BinarySerializer<T>::serialize(result, target);
}

template<typename T>
void binaryDeserialize(unsigned char *bytes, T &result) {
  BinaryDeserializer<T>::deserialize(bytes, result);
}

}
