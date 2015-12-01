#pragma once
#include <memory>

#include "declarations.h"

namespace relevanced {
namespace document_processing_worker {

class DocumentProcessorIf {
 public:
  virtual models::ProcessedDocument process(models::Document &doc) = 0;

  virtual std::shared_ptr<models::ProcessedDocument>
    processNew(models::Document &doc) = 0;

  virtual std::shared_ptr<models::ProcessedDocument>
    processNew(std::shared_ptr<models::Document> doc) = 0;

  virtual ~DocumentProcessorIf() = default;
};

class DocumentProcessor : public DocumentProcessorIf {
 protected:
  std::shared_ptr<stemmer::StemmerManagerIf> stemmerManager_;
  std::shared_ptr<stopwords::StopwordFilterIf> stopwordFilter_;
  std::shared_ptr<util::ClockIf> clock_;

  void process_(models::Document&, models::ProcessedDocument*);

  void process_(models::Document&, std::shared_ptr<models::ProcessedDocument>);

 public:
  DocumentProcessor(
    std::shared_ptr<stemmer::StemmerManagerIf>,
    std::shared_ptr<stopwords::StopwordFilterIf>,
    std::shared_ptr<util::ClockIf>
  );

  std::shared_ptr<models::ProcessedDocument>
    processNew(models::Document&) override;

  std::shared_ptr<models::ProcessedDocument>
    processNew(std::shared_ptr<models::Document>) override;

  models::ProcessedDocument process(models::Document&) override;
};

} // document_processing_worker
} // relevanced