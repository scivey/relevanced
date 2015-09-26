#pragma once
#include <cstdlib>
#include "bytes.h"
#include <glog/logging.h>
namespace serialization {

template<typename T>
struct Deserializer {
  static void deserialize(unsigned char *bytes, T &result) {
    LOG(INFO) << "default deserialization!";
  }
};

template<typename T>
struct Serializer {
  static size_t serialize(unsigned char **result, T &target) {
    unsigned char *res = (unsigned char*) malloc(1);
    *result = res;
    LOG(INFO) << "default serialization!";
    return 1;
  }
};

template<typename T>
size_t serialize(unsigned char **result, T &target) {
  return Serializer<T>::serialize(result, target);
}

template<typename T>
void deserialize(unsigned char *bytes, T &result) {
  Deserializer<T>::deserialize(bytes, result);
}

}
