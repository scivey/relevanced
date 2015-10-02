#include <string>
#include <sstream>
#include <cmath>
#include <map>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace std;

namespace relevanced {
namespace util {

string getUuid() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  ostringstream oss;
  oss << uuid;
  return oss.str();
}

bool isOnlyAscii(const string &text) {
  for (size_t i = 0; i < text.size(); i++) {
    unsigned char c = text.at(i);
    if (c > 127) {
      return false;
    }
  }
  return true;
}

} // util
} // relevanced
