#pragma once
#include <exception>

namespace persistence {

class DocumentDoesNotExist: public std::exception {};
class CentroidDoesNotExist: public std::exception {};
class CentroidAlreadyExists: public std::exception {};

} // persistence
