#pragma once
#include <exception>

namespace persistence {
namespace execeptions {

class DocumentDoesNotExist: public std::exception {};
class DocumentAlreadyExists: public std::exception {};
class CentroidDoesNotExist: public std::exception {};
class CentroidAlreadyExists: public std::exception {};

} // exceptions
} // persistence
