#include <string>
#include <cstring>
#include <memory>

#include "libstemmer.h"
#include "stemmer/ThreadSafeStemmerManager.h"
#include "stopwords/StopwordFilter.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "util/Clock.h"
#include "models/Document.h"
#include "models/ProcessedDocument.h"
#include "gen-cpp2/RelevancedProtocol_types.h"


using namespace relevanced;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::util;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using relevanced::thrift_protocol::Language;

using namespace std;

void check() {
  shared_ptr<StemmerManagerIf> stemPtr(
    new ThreadSafeStemmerManager
  );
  shared_ptr<ClockIf> clockPtr(
    new Clock
  );
  shared_ptr<StopwordFilterIf> stopwordfilter(
    new StopwordFilter
  );
  DocumentProcessor processor(stemPtr, stopwordfilter, clockPtr);
  for (size_t i = 0; i < 1000; i++) {
    Document document(
      "doc-id", "this is some text about fish", Language::EN
    );
    processor.process(document);
  }
}

int main() {
  check();
}

