#pragma once

#include <memory>
#include <map>
#include <folly/ThreadLocal.h>
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "stemmer/StemmerIf.h"
#include "stemmer/Utf8Stemmer.h"

#include "stemmer/ThreadSafeStemmerManager.h"
using namespace std;
using relevanced::thrift_protocol::Language;

namespace relevanced {
namespace stemmer {

shared_ptr<StemmerIf> ThreadSafeStemmerManager::getStemmer(Language lang) {
  auto found = stemmers_->find(lang);
  if (found != stemmers_->end()) {
    return found->second;
  }
  std::shared_ptr<StemmerIf> created(
    new Utf8Stemmer(lang)
  );
  stemmers_->insert(make_pair(lang, created));
  return created;
}

}
}

