#include <memory>
#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>

#include "document_processing_worker/DocumentProcessingWorker.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "models/WordVector.h"
#include "models/ProcessedDocument.h"
#include "models/Document.h"
#include "util/Clock.h"
#include "util/Sha1Hasher.h"

namespace relevanced {
namespace document_processing_worker {

using namespace std;
using namespace folly;
using wangle::FutureExecutor;
using wangle::CPUThreadPoolExecutor;
using models::Document;
using models::WordVector;
using models::ProcessedDocument;


DocumentProcessingWorker::DocumentProcessingWorker(
    shared_ptr<DocumentProcessorIf> processor,
    shared_ptr<util::Sha1HasherIf> hasher,
    shared_ptr<FutureExecutor<CPUThreadPoolExecutor>> threadPool)
    : processor_(processor), hasher_(hasher), threadPool_(threadPool) {}


using FutureDoc = Future<shared_ptr<ProcessedDocument>>;

FutureDoc DocumentProcessingWorker::processNew(
    shared_ptr<Document> doc) {
  return threadPool_->addFuture([this, doc]() {
    auto result = processor_->processNew(doc);
    result->sha1Hash.assign(hasher_->hash(doc->text));
    return result;
  });
}

FutureDoc DocumentProcessingWorker::processNewWithoutHash(
    shared_ptr<Document> doc) {
  return threadPool_->addFuture([this, doc]() {
    return processor_->processNew(doc);
  });
}


} // document_processing_worker
} // relevanced
