#pragma once
#include <cstdlib>
#include <string>
#include "declarations.h"

namespace relevanced {
namespace serialization {

void binarySerialize(std::string &result, models::ProcessedDocument &target);
void binarySerialize(std::string &result, models::WordVector &target);
void binarySerialize(std::string &result, models::Centroid &target);

void binaryDeserialize(std::string &data, models::ProcessedDocument *result);
void binaryDeserialize(std::string &data, models::WordVector *result);
void binaryDeserialize(std::string &data, models::Centroid *result);

void jsonSerialize(std::string &result, models::ProcessedDocument *target);
void jsonSerialize(std::string &result, models::WordVector *target);
void jsonSerialize(std::string &result, models::Centroid *target);

void jsonDeserialize(std::string &data, models::ProcessedDocument *result);
void jsonDeserialize(std::string &data, models::WordVector *result);
void jsonDeserialize(std::string &data, models::Centroid *result);


} // serialization
} // relevanced
