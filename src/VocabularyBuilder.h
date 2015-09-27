#pragma once
#include <string>
#include <set>
#include <memory>
#include "Vocabulary.h"

class VocabularyBuilder {
  std::set<std::string> words_;
public:
  VocabularyBuilder();
  void addDocument(ProcessedDocument *document);
  std::shared_ptr<Vocabulary> build();
};
