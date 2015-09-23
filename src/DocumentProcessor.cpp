#include <memory>
#include <string>
#include <map>
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "Document.h"
using namespace std;

void DocumentProcessor::process_(const Document &doc, ProcessedDocument *result) {
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

ProcessedDocument DocumentProcessor::process(const Document &doc) {
  ProcessedDocument processed(doc.id);
  process_(doc, &processed);
  return processed;
}

ProcessedDocument* DocumentProcessor::processNew(const Document &doc) {
  auto result = new ProcessedDocument(doc.id);
  process_(doc, result);
  return result;
}
