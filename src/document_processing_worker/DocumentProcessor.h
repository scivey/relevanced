#pragma once
#include <memory>
#include "stemmer/StemmerIf.h"
#include "stopwords/StopwordFilter.h"
#include "tokenizer/Tokenizer.h"
#include "models/Document.h"
#include "models/WordVector.h"
#include "util/util.h"

namespace relevanced {
namespace document_processing_worker {

class DocumentProcessorIf {
public:
  virtual models::WordVector process(const models::Document &doc) = 0;
  virtual std::shared_ptr<models::WordVector> processNew(const models::Document &doc) = 0;
  virtual std::shared_ptr<models::WordVector> processNew(std::shared_ptr<models::Document> doc) = 0;
  virtual ~DocumentProcessorIf() = default;
};

class DocumentProcessor: public DocumentProcessorIf {
protected:
  void process_(const models::Document &doc, models::WordVector* result);
  void process_(const models::Document &doc, std::shared_ptr<models::WordVector> result);
  std::shared_ptr<stemmer::StemmerIf> stemmer_;
  std::shared_ptr<tokenizer::TokenizerIf> tokenizer_;
  std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter_;
public:
  DocumentProcessor(
    std::shared_ptr<tokenizer::TokenizerIf> tokenizer,
    std::shared_ptr<stemmer::StemmerIf> stemmer,
    std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter
  ) : stemmer_(stemmer), tokenizer_(tokenizer), stopwordFilter_(stopwordFilter) {}
  std::shared_ptr<models::WordVector> processNew(const models::Document &doc) override;
  std::shared_ptr<models::WordVector> processNew(std::shared_ptr<models::Document> doc) override;
  models::WordVector process(const models::Document &doc) override;

};

} // document_processing_worker
} // relevanced