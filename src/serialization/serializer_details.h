#pragma once
#include <cstdlib>
#include <boost/static_assert.hpp>
#include <string>
#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <thrift/lib/cpp2/protocol/Serializer.h>
#include <glog/logging.h>

namespace relevanced {
namespace serialization {

template <typename T>
struct BinaryDeserializer {
  static void deserialize(std::string &data, T *result) {
    ((void) data);
    ((void) result);

    LOG(INFO) << "default binary deserialization";
  }
};

template <typename T>
struct BinarySerializer {
  static void serialize(std::string &result, T &target) {
    ((void) result);
    ((void) target);

    LOG(INFO) << "default binary serialization";
  }
};

template <typename T>
void binarySerializeAny(std::string &result, T &target) {
  BinarySerializer<T>::serialize(result, target);
}

template <typename T>
void binaryDeserializeAny(std::string &data, T *result) {
  BinaryDeserializer<T>::deserialize(data, result);
}

template <typename T>
void thriftBinarySerialize(std::string &result, T &target) {
  apache::thrift::Serializer<apache::thrift::BinaryProtocolReader,
                             apache::thrift::BinaryProtocolWriter> serializer;
  serializer.serialize<T>(target, &result);
}

template <typename T>
void thriftBinaryDeserialize(std::string &data, T &target) {
  apache::thrift::Serializer<apache::thrift::BinaryProtocolReader,
                             apache::thrift::BinaryProtocolWriter> serializer;
  serializer.deserialize<T>(data, target);
}

template <typename T>
struct JsonSerializer {
  static void serialize(std::string &result, T *) {
    LOG(INFO) << "default json serialization";
    result = "";
  }
};

template <typename T>
struct JsonDeserializer {
  static void deserialize(const std::string &, T *) {
    LOG(INFO) << "default json deserialization";
  }
};

template <typename T>
void jsonSerializeAny(std::string &result, T *target) {
  JsonSerializer<T>::serialize(result, target);
}

template <typename T>
void jsonDeserializeAny(const std::string &jsonStr, T *result) {
  JsonDeserializer<T>::deserialize(jsonStr, result);
}

} // serialization
} // relevanced
