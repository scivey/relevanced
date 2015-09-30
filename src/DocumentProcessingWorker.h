#pragma once

#include <memory>
#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "Document.h"

class DocumentProcessingWorkerIf {
public:
  virtual folly::Future<std::shared_ptr<ProcessedDocument>> processNew(std::shared_ptr<Document>) = 0;
};

class DocumentProcessingWorker: public DocumentProcessingWorkerIf {
  std::shared_ptr<DocumentProcessorIf> processor_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
public:
  DocumentProcessingWorker(
    std::shared_ptr<DocumentProcessorIf>,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
  );
  folly::Future<std::shared_ptr<ProcessedDocument>> processNew(std::shared_ptr<Document>) override;
};
