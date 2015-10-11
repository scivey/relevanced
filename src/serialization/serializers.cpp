#include <string>

#include "serialization/serializers.h"
#include "serialization/serializer_details.h"
#include "serialization/serializer_WordVector.h"
#include "serialization/serializer_Centroid.h"
#include "serialization/serializer_ProcessedDocument.h"
#include "declarations.h"

namespace relevanced {
namespace serialization {

void binarySerialize(std::string &result, models::ProcessedDocument &target) {
  binarySerializeAny<models::ProcessedDocument>(result, target);
}

void binarySerialize(std::string &result, models::WordVector &target) {
  binarySerializeAny<models::WordVector>(result, target);
}

void binarySerialize(std::string &result, models::Centroid &target) {
  binarySerializeAny<models::Centroid>(result, target);
}

void binaryDeserialize(std::string &data, models::ProcessedDocument *result) {
  binaryDeserializeAny<models::ProcessedDocument>(data, result);
}

void binaryDeserialize(std::string &data, models::WordVector *result) {
  binaryDeserializeAny<models::WordVector>(data, result);
}

void binaryDeserialize(std::string &data, models::Centroid *result) {
  binaryDeserializeAny<models::Centroid>(data, result);
}

void jsonSerialize(std::string &result, models::ProcessedDocument *target) {
  jsonSerializeAny<models::ProcessedDocument>(result, target);
}

void jsonSerialize(std::string &result, models::WordVector *target) {
  jsonSerializeAny<models::WordVector>(result, target);
}

void jsonSerialize(std::string &result, models::Centroid *target) {
  jsonSerializeAny<models::Centroid>(result, target);
}

void jsonDeserialize(std::string &data, models::ProcessedDocument *result) {
  jsonDeserializeAny<models::ProcessedDocument>(data, result);
}

void jsonDeserialize(std::string &data, models::WordVector *result) {
  jsonDeserializeAny<models::WordVector>(data, result);
}

void jsonDeserialize(std::string &data, models::Centroid *result) {
  jsonDeserializeAny<models::Centroid>(data, result);
}

} // serialization
} // relevanced
