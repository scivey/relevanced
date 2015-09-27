#include "Vocabulary.h"
#include <map>
#include <string>
#include <eigen3/Eigen/Sparse>
#include "ProcessedDocument.h"

Vocabulary::Vocabulary(){};
Vocabulary::Vocabulary(map<string, size_t> wordIndices): wordIndices_(wordIndices){}

size_t Vocabulary::size() {
  return wordIndices_.size();
}

Eigen::SparseVector<double> Vocabulary::vecOfDocument(ProcessedDocument *document) {
  Eigen::SparseVector<double> output(wordIndices_.size());
  for (auto &elem: document->normalizedWordCounts) {
    if (wordIndices_.find(elem.first) != wordIndices_.end()) {
      auto wordIndex = wordIndices_[elem.first];
      output.insert(wordIndex) = elem.second;
    }
  }
  return output;
}
