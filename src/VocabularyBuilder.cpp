#include "VocabularyBuilder.h"
#include <map>
#include <cassert>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include "Vocabulary.h"
#include "util.h"
using namespace std;
VocabularyBuilder::VocabularyBuilder(){}

void VocabularyBuilder::addDocument(ProcessedDocument *document) {
  for (auto &wordScore: document->normalizedWordCounts) {
    words_.insert(wordScore.first);
  }
}

shared_ptr<Vocabulary> VocabularyBuilder::build() {
  auto keys = util::vecOfSet(words_);
  std::sort(keys.begin(), keys.end());
  size_t count = keys.size();
  map<string, size_t> wordIndices;
  for (size_t i = 0; i < count; i++) {
    wordIndices.insert(
      make_pair(keys.at(i), i)
    );
  }
  return std::make_shared<Vocabulary>(std::move(wordIndices));
}

