#include <string>
#include <sstream>
#include <cmath>
#include <map>

#include <eigen3/Eigen/Sparse>
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

double vectorMag(const Eigen::SparseVector<double> &vec, size_t count) {
  double accum = 0.0;
  for (Eigen::SparseVector<double>::InnerIterator it(vec); it; ++it) {
    accum += pow(it.value(), 2);
  }
  return sqrt(accum);
}

double sparseDot(Eigen::SparseVector<double> &v1, Eigen::SparseVector<double> &v2) {
  map<size_t, double> indices;
  double dotProd = 0.0;
  for (Eigen::SparseVector<double>::InnerIterator it(v1); it; ++it) {
    indices.insert(make_pair(it.index(), it.value()));
  }
  for (Eigen::SparseVector<double>::InnerIterator it(v2); it; ++it) {
    if (indices.find(it.index()) != indices.end()) {
      dotProd += (indices[it.index()] * it.value());
    }
  }
  return dotProd;
}

} // util
