#include "document_processing_worker/DocumentProcessingWorker.h"
#include "models/WordVector.h"
#include "models/Document.h"

#include <memory>
#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

namespace relevanced {
namespace document_processing_worker {

using namespace std;
using namespace folly;
using wangle::FutureExecutor;
using wangle::CPUThreadPoolExecutor;
using models::Document;
using models::WordVector;

DocumentProcessingWorker::DocumentProcessingWorker(
  shared_ptr<DocumentProcessorIf> processor,
  shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool
) : processor_(processor), threadPool_(threadPool) {}


Future<shared_ptr<WordVector>> DocumentProcessingWorker::processNew(shared_ptr<Document> doc) {
  return threadPool_->addFuture([this, doc](){
    return processor_->processNew(doc);
  });
}

} // document_processing_worker
} // relevanced
