#include "DocumentProcessingWorker.h"
#include "ProcessedDocument.h"
#include "Document.h"

#include <memory>
#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

using namespace std;
using namespace folly;

DocumentProcessingWorker::DocumentProcessingWorker(shared_ptr<DocumentProcessorIf> processor)
  : processor_(processor){}


Future<shared_ptr<ProcessedDocument>> DocumentProcessingWorker::processNew(shared_ptr<Document> doc) {
  return threadPool_.addFuture([this, doc](){
    return processor_->processNew(*doc);
  });
}
