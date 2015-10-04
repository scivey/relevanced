#pragma once
#include <cstdlib>
#include <boost/static_assert.hpp>
#include <string>
#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <thrift/lib/cpp2/protocol/Serializer.h>
#include <glog/logging.h>
#include "bytes.h"

namespace relevanced {
namespace serialization {

template<typename T>
struct BinaryDeserializer {
  static void deserialize(std::string &data, T *result) {
    ((void) data);
    ((void) result);

    LOG(INFO) << "default binary deserialization";
  }
};

template<typename T>
struct BinarySerializer {
  static void serialize(std::string &result, T &target) {
    ((void) result);
    ((void) target);

    LOG(INFO) << "default binary serialization";
  
  }
};

template<typename T>
void binarySerialize(std::string &result, T &target) {
  BinarySerializer<T>::serialize(result, target);
}

template<typename T>
void binaryDeserialize(std::string &data, T *result) {
  BinaryDeserializer<T>::deserialize(data, result);
}

template<typename T>
void thriftBinarySerialize(std::string &result, T &target) {
  apache::thrift::Serializer<
    apache::thrift::BinaryProtocolReader,
    apache::thrift::BinaryProtocolWriter
  > serializer;
  serializer.serialize<T>(target, &result);
}

template<typename T>
void thriftBinaryDeserialize(std::string &data, T &target) {
  apache::thrift::Serializer<
    apache::thrift::BinaryProtocolReader,
    apache::thrift::BinaryProtocolWriter
  > serializer;
  serializer.deserialize<T>(data, target);
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

} // serialization
} // relevanced
