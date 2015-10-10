#include <memory>
#include <string>
#include <map>
#include <cmath>
#include "DocumentProcessor.h"
#include "models/Document.h"
#include "models/ProcessedDocument.h"
#include "models/WordVector.h"
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "util/Clock.h"
#include "util/util.h"


namespace relevanced {
namespace document_processing_worker {

using models::Document;
using models::ProcessedDocument;
using namespace std;
using util::UniquePointer;

void DocumentProcessor::process_(const Document &doc, ProcessedDocument *result) {
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
  result->wordVector.magnitude = magnitude;
  result->wordVector.documentWeight = 1.0;
  result->wordVector.scores = std::move(wordCounts);
  auto timestamp = clock_->getEpochTime();
  result->created = timestamp;
  result->updated = timestamp;
}

void DocumentProcessor::process_(const Document &doc, shared_ptr<ProcessedDocument> result) {
  return process_(doc, result.get());
}

ProcessedDocument DocumentProcessor::process(const Document &doc) {
  ProcessedDocument processed(doc.id);
  process_(doc, &processed);
  return processed;
}

shared_ptr<ProcessedDocument> DocumentProcessor::processNew(const Document &doc) {
  auto result = std::make_shared<ProcessedDocument>(doc.id);
  process_(doc, result);
  return result;
}

shared_ptr<ProcessedDocument> DocumentProcessor::processNew(shared_ptr<Document> doc) {
  auto result = std::make_shared<ProcessedDocument>(doc->id);
  auto d2 = *doc;
  process_(d2, result);
  return result;
}

} // document_processing_worker
} // relevanced