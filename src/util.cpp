#include <string>
#include <sstream>
#include <cmath>

#include <eigen3/Eigen/Dense>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace std;

namespace util {

string getUuid() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  ostringstream oss;
  oss << uuid;
  return oss.str();
}

double vectorMag(const Eigen::VectorXd &vec, size_t count) {
  double accum = 0.0;
  for (size_t i = 0; i < count; i++) {
    accum += pow(vec(i), 2);
  }
  return sqrt(accum);
}


} // util
