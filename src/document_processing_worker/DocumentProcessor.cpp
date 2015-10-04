#include <memory>
#include <string>
#include <map>
#include <cmath>
#include "DocumentProcessor.h"
#include "models/WordVector.h"
#include "models/Document.h"
#include "util/util.h"

namespace relevanced {
namespace document_processing_worker {

using models::Document;
using models::WordVector;
using namespace std;
using util::UniquePointer;

void DocumentProcessor::process_(const Document &doc, WordVector *result) {
  // computes normalized frequency for each term in the document:
  // raw frequency divided by total number of (non-stopword) terms
  map<string, double> wordCounts;
  size_t totalWords = 0;
  for (auto &word: tokenizer_->tokenize(doc.text)) {
    stemmer_->stemInPlace(word);
    if (!stopwordFilter_->isStopword(word)) {
      totalWords++;
      if (wordCounts.find(word) == wordCounts.end()) {
        wordCounts[word] = 1.0;
      } else {
        wordCounts[word] += 1.0;
      }
    }
  }
  double dTotal = (double) totalWords;
  double magnitude = 0.0;
  for (auto &elem: wordCounts) {
    double normalized = elem.second / dTotal;
    wordCounts[elem.first] = normalized;
    magnitude += pow(normalized, 2);
  }
  magnitude = sqrt(magnitude);
  result->id = doc.id;
  result->magnitude = magnitude;
  result->documentWeight = 1.0;
  result->scores = std::move(wordCounts);
}

void DocumentProcessor::process_(const Document &doc, shared_ptr<WordVector> result) {
  return process_(doc, result.get());
}

WordVector DocumentProcessor::process(const Document &doc) {
  WordVector processed(doc.id);
  process_(doc, &processed);
  return processed;
}

shared_ptr<WordVector> DocumentProcessor::processNew(const Document &doc) {
  auto result = std::make_shared<WordVector>(doc.id);
  process_(doc, result);
  return result;
}

shared_ptr<WordVector> DocumentProcessor::processNew(shared_ptr<Document> doc) {
  auto result = std::make_shared<WordVector>(doc->id);
  auto d2 = *doc;
  process_(d2, result);
  return result;
}

} // document_processing_worker
} // relevanced