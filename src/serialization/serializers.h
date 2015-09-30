#pragma once
#include <cstdlib>
#include <cassert>
#include <boost/static_assert.hpp>
#include "bytes.h"
#include <glog/logging.h>
namespace serialization {

template<typename T>
struct BinaryDeserializer {
  static void deserialize(unsigned char *bytes, T &result) {
    ((void) bytes);
    ((void) result);

    LOG(INFO) << "default binary deserialization";
  }
};

template<typename T>
struct BinarySerializer {
  static size_t serialize(unsigned char **result, T &target) {
    ((void) result);
    ((void) target);

    LOG(INFO) << "default binary serialization";
    return 0;
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

template<typename T>
struct JsonSerializer {
  static string serialize(T *target) {
    ((void) target);
    LOG(INFO) << "default json serialization";
    return "";
  }
};

template<typename T>
struct JsonDeserializer {
  static T deserialize(const std::string &jsonStr) {
    ((void) jsonStr);
    LOG(INFO) << "default json deserialization";
    T t;
    return t;
  }
};

template<typename T>
string jsonSerialize(T *target) {
  return JsonSerializer<T>::serialize(target);
}

template<typename T>
T jsonDeserialize(const std::string &jsonStr) {
  return JsonDeserializer<T>::deserialize(jsonStr);
}


}
