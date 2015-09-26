#pragma once
#include <memory>
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "Document.h"
#include "ProcessedDocument.h"
#include "util.h"

class DocumentProcessorIf {
public:
  virtual ProcessedDocument process(const Document &doc) = 0;
  virtual std::shared_ptr<ProcessedDocument> processNew(const Document &doc) = 0;
  virtual util::UniquePointer<ProcessedDocument> processUnique(const Document &doc) = 0;
};

class DocumentProcessor: public DocumentProcessorIf {
protected:
  void process_(const Document &doc, ProcessedDocument* result);
  void process_(const Document &doc, std::shared_ptr<ProcessedDocument> result);
  std::shared_ptr<stemmer::StemmerIf> stemmer_;
  std::shared_ptr<tokenizer::TokenizerIf> tokenizer_;
  std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter_;
public:
  DocumentProcessor(
    std::shared_ptr<tokenizer::TokenizerIf> tokenizer,
    std::shared_ptr<stemmer::StemmerIf> stemmer,
    std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter
  ) : stemmer_(stemmer), tokenizer_(tokenizer), stopwordFilter_(stopwordFilter) {}
  std::shared_ptr<ProcessedDocument> processNew(const Document &doc) override;
  util::UniquePointer<ProcessedDocument> processUnique(const Document &doc) override;
  ProcessedDocument process(const Document &doc) override;

};
