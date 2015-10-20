#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <tuple>
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
#include "text_util/ScoredWord.h"
#include "text_util/StringView.h"
#include "text_util/DestructiveConllTokenIterator.h"

namespace relevanced {
namespace document_processing_worker {

using models::Document;
using models::ProcessedDocument;
using namespace std;
using util::UniquePointer;
using namespace relevanced::text_util;

void DocumentProcessor::process_(const Document &doc, ProcessedDocument *result) {
  DestructiveConllTokenIterator it(doc.text.c_str(), doc.text.size());
  std::vector<ScoredWord> scoredWords;
  scoredWords.reserve(200);
  std::unordered_map<StringView, size_t> wordsByStr;
  const char *cStr = doc.text.c_str();
  size_t totalWords {0};
  std::tuple<bool, size_t, size_t> tokenOffsets;
  while (it.next(tokenOffsets)) {
    if (!std::get<0>(tokenOffsets)) {
      break;
    }
    size_t startPos = ::std::get<1>(tokenOffsets);
    size_t len = ::std::get<2>(tokenOffsets) - startPos;
    if (len < 3) {
      continue;
    }
    const char *tokenStart = cStr + startPos;
    char firstChar = *tokenStart;
    if (it.isAsciiNumeric(firstChar)) {
      continue;
    }
    if (it.isPunctuation(firstChar)) {
      continue;
    }
    len = utf8Stemmer_.getStemPos(tokenStart, len);
    if (len > 23) {
      len = 23;
    }
    StringView view(tokenStart, len);
    auto existingIndex = wordsByStr.find(view);
    if (existingIndex == wordsByStr.end()) {
      uint8_t wordLen = (uint8_t) len;
      ScoredWord scored(tokenStart, wordLen);
      if (stopwordFilter_->isStopword(scored.word)) {
        continue;
      }
      scoredWords.push_back(scored);
      wordsByStr[view] = scoredWords.size() - 1;
    } else {
      auto existing = &scoredWords.at(existingIndex->second);
      existing->score += 1;
    }
    totalWords++;
  }
  double dTotal = (double) totalWords;
  double magnitude = 0.0;
  for (auto &elem: scoredWords) {
    double normalized = elem.score / dTotal;
    elem.score = normalized;
    magnitude += pow(normalized, 2);
  }
  magnitude = sqrt(magnitude);
  result->scoredWords = std::move(scoredWords);
  result->magnitude = magnitude;
  result->id = doc.id;
  auto timestamp = clock_->getEpochTime();
  result->created = timestamp;
  result->updated = timestamp;
}


void DocumentProcessor::process_(const Document &doc,
                                 shared_ptr<ProcessedDocument> result) {
  return process_(doc, result.get());
}

ProcessedDocument DocumentProcessor::process(const Document &doc) {
  ProcessedDocument processed(doc.id);
  process_(doc, &processed);
  return processed;
}

shared_ptr<ProcessedDocument> DocumentProcessor::processNew(
    const Document &doc) {
  auto result = std::make_shared<ProcessedDocument>(doc.id);
  process_(doc, result);
  return result;
}

shared_ptr<ProcessedDocument> DocumentProcessor::processNew(
    shared_ptr<Document> doc) {
  auto result = std::make_shared<ProcessedDocument>(doc->id);
  auto d2 = *doc;
  process_(d2, result);
  return result;
}

} // document_processing_worker
} // relevanced