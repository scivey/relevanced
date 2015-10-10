#pragma once
#include <memory>

#include "declarations.h"

namespace relevanced {
namespace document_processing_worker {

class DocumentProcessorIf {
public:
  virtual models::ProcessedDocument process(const models::Document &doc) = 0;
  virtual std::shared_ptr<models::ProcessedDocument> processNew(const models::Document &doc) = 0;
  virtual std::shared_ptr<models::ProcessedDocument> processNew(std::shared_ptr<models::Document> doc) = 0;
  virtual ~DocumentProcessorIf() = default;
};

class DocumentProcessor: public DocumentProcessorIf {
protected:
  void process_(const models::Document &doc, models::ProcessedDocument* result);
  void process_(const models::Document &doc, std::shared_ptr<models::ProcessedDocument> result);
  std::shared_ptr<stemmer::StemmerIf> stemmer_;
  std::shared_ptr<tokenizer::TokenizerIf> tokenizer_;
  std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter_;
  std::shared_ptr<util::ClockIf> clock_;
public:
  DocumentProcessor(
    std::shared_ptr<tokenizer::TokenizerIf> tokenizer,
    std::shared_ptr<stemmer::StemmerIf> stemmer,
    std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter,
    std::shared_ptr<util::ClockIf> clock
  ) : stemmer_(stemmer), tokenizer_(tokenizer), stopwordFilter_(stopwordFilter), clock_(clock) {}
  std::shared_ptr<models::ProcessedDocument> processNew(const models::Document &doc) override;
  std::shared_ptr<models::ProcessedDocument> processNew(std::shared_ptr<models::Document> doc) override;
  models::ProcessedDocument process(const models::Document &doc) override;

};

} // document_processing_worker
} // relevanced