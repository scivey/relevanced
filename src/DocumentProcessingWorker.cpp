#include "DocumentProcessingWorker.h"
#include "ProcessedDocument.h"
#include "Document.h"

#include <memory>
#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

using namespace std;
using namespace folly;
using wangle::FutureExecutor;
using wangle::CPUThreadPoolExecutor;

DocumentProcessingWorker::DocumentProcessingWorker(
  shared_ptr<DocumentProcessorIf> processor,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
) : processor_(processor), threadPool_(threadPool) {}


Future<shared_ptr<ProcessedDocument>> DocumentProcessingWorker::processNew(shared_ptr<Document> doc) {
  return threadPool_->addFuture([this, doc](){
    return processor_->processNew(doc);
  });
}
