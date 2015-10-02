#pragma once
#include <exception>

namespace relevanced {
namespace persistence {
namespace exceptions {

class DocumentDoesNotExist: public std::exception {};
class DocumentAlreadyExists: public std::exception {};
class CentroidDoesNotExist: public std::exception {};
class CentroidAlreadyExists: public std::exception {};

} // exceptions
} // persistence
} // relevanced

