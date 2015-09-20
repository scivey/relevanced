#include <iostream>
#include <glog/logging.h>
#include <folly/Format.h>
#include <folly/Format.h>
#include <eigen3/Eigen/Dense>
using namespace std;
using namespace folly;
using namespace Eigen;
int main() {
  LOG(INFO) << "start!!";
  MatrixXd m(2, 2);
  m(0, 0) = 3;
  m(1, 0) = 2.5;
  m(0, 1) = -1;
  m(1, 1) = m(1, 0) + m(0, 1);
  LOG(INFO) << "mat: ";
  cout << m << endl;
  VectorXd v(2);
  v(0) = 4;
  v(1) = v(0) - 1;
  LOG(INFO) << "vec: ";
  cout << v << endl;

  VectorXd v2(2);
  v2(0) = 1;
  v2(1) = 0.1;
  LOG(INFO) << "v2: ";
  cout << v2 << endl;

  auto res = v * v2.transpose();
  LOG(INFO) << "res: ";
  cout << res << endl;
  LOG(INFO) << "end!!";
}
