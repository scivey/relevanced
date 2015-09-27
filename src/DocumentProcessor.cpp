#include <memory>
#include <string>
#include <map>
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "Document.h"
#include "util.h"

using namespace std;
using util::UniquePointer;

void DocumentProcessor::process_(const Document &doc, ProcessedDocument *result) {
  // computes normalized frequency for each term in the document:
  // raw frequency divided by total number of (non-stopword) terms
  map<string, double> wordCounts;
  size_t totalWords;
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
  for (auto &elem: wordCounts) {
    wordCounts[elem.first] = elem.second / dTotal;
  }
  result->normalizedWordCounts = std::move(wordCounts);
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


UniquePointer<ProcessedDocument> DocumentProcessor::processUnique(const Document &doc) {
  ProcessedDocument *result = new ProcessedDocument(doc.id);
  process_(doc, result);
  return UniquePointer<ProcessedDocument>(result);
}
