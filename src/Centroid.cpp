#include <string>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/DynamicConverter.h>

#include "Vocabulary.h"
#include "ProcessedDocument.h"
#include "Centroid.h"
#include "util.h"
using namespace std;
using namespace folly;

Centroid::Centroid(): center_(1){}

Centroid::Centroid(Eigen::SparseVector<double> center, std::shared_ptr<Vocabulary> vocabulary)
  : center_(center), vocabulary_(vocabulary) {}

double Centroid::score(ProcessedDocument *document) {
  auto docVec = vocabulary_->vecOfDocument(document);
  const size_t vocabSize = vocabulary_->size();
  map<size_t, double> indices;
  double dotProd = 0.0;
  for (Eigen::SparseVector<double>::InnerIterator it(docVec); it; ++it) {
    indices.insert(make_pair(it.index(), it.value()));
  }
  for (Eigen::SparseVector<double>::InnerIterator it(center_); it; ++it) {
    if (indices.find(it.index()) != indices.end()) {
      dotProd += (indices[it.index()] * it.value());
    }
  }
  double mag1 = util::vectorMag(center_, vocabSize);
  double mag2 = util::vectorMag(docVec, vocabSize);
  return dotProd / (mag1 * mag2);
}
