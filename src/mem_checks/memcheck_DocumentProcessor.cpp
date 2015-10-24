#include <string>
#include <cstring>
#include <memory>

#include "libstemmer.h"
#include "stemmer/ThreadSafeUtf8Stemmer.h"
#include "stopwords/StopwordFilter.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "util/Clock.h"
#include "models/Document.h"
#include "models/ProcessedDocument.h"


using namespace relevanced;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::util;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using namespace std;

void check() {
  shared_ptr<StemmerIf> stemmer(
    new ThreadSafeUtf8Stemmer
  );
  shared_ptr<ClockIf> clockPtr(
    new Clock
  );
  shared_ptr<StopwordFilterIf> stopwordfilter(
    new StopwordFilter
  );
  DocumentProcessor processor(stemmer, stopwordfilter, clockPtr);
  for (size_t i = 0; i < 1000; i++) {
    Document document("doc-id", "this is some text about fish");
    processor.process(document);
  }
}

int main() {
  check();
}

