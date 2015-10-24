#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "models/ProcessedDocument.h"
#include "centroid_update_worker/DocumentAccumulator.h"

using namespace std;

namespace relevanced {
namespace centroid_update_worker {
using models::ProcessedDocument;

void DocumentAccumulator::addDocument(ProcessedDocument* document) {
  staleMagnitude_ = true;
  documentCount_++;
  for (auto &elem : document->scoredWords) {
    string key = elem.word;
    if (scores_.find(key) == scores_.end()) {
      scores_[key] = elem.score;
    } else {
      scores_[key] += elem.score;
    }
  }
}

std::unordered_map<std::string, double>&& DocumentAccumulator::getScores() {
  return std::move(scores_);
}

double DocumentAccumulator::getMagnitude() {
  if (staleMagnitude_) {
    double centroidMagnitude = 0.0;
    for (auto &elem : scores_) {
      centroidMagnitude += pow(elem.second, 2);
    }
    centroidMagnitude = sqrt(centroidMagnitude);
    magnitude_ = centroidMagnitude;
    staleMagnitude_ = false;
  }
  return magnitude_;
}

size_t DocumentAccumulator::getCount() {
  return documentCount_;
}

} // centroid_update_worker
} // relevanced
