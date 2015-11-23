#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <folly/Format.h>
#include "libstemmer.h"
#include "stemmer/ThreadSafeStemmerManager.h"
#include "stopwords/StopwordFilter.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "persistence/InMemoryRockHandle.h"
#include "persistence/SyncPersistence.h"
#include "util/Clock.h"
#include "util/util.h"
#include "testing/TestHelpers.h"
#include "models/Document.h"
#include "models/ProcessedDocument.h"
#include "text_util/ScoredWord.h"

using namespace relevanced;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::util;
using namespace relevanced::models;
using namespace relevanced::persistence;
using namespace relevanced::text_util;

using namespace std;

void check() {
  InMemoryRockHandle rockHandle {"foo"};
  shared_ptr<ClockIf> clockPtr(
    new Clock
  );
  UniquePointer<RockHandleIf> rockPtr(
    &rockHandle, NonDeleter<RockHandleIf>()
  );
  SyncPersistence syncDb(clockPtr, std::move(rockPtr));
  for (size_t i = 0; i < 1000; i++) {
    auto id = folly::sformat("doc-{}", i);
    ProcessedDocument pdoc(
      id,
      vector<ScoredWord> {
        ScoredWord("fish", 4, 0.5),
        ScoredWord("cats", 4, 0.5)
      },
      1.0
    );
    syncDb.saveDocument(&pdoc);
  }
}

int main() {
  check();
}

