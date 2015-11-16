#include <string>
#include <cstring>
#include "stemmer/Utf8Stemmer.h"
#include "libstemmer.h"
#include "gen-cpp2/RelevancedProtocol_types.h"
#include "util/util.h"

using relevanced::thrift_protocol::Language;

namespace relevanced {
namespace stemmer {

Utf8Stemmer::Utf8Stemmer(Language lang): language_(lang) {
  const char *countryCode = util::countryCodeOfThriftLanguage(language_);
  if (countryCode == "UNKNOWN") {
    countryCode = "en";
  }
  stemmer_ = sb_stemmer_new(countryCode, "UTF_8");
}

size_t Utf8Stemmer::getStemPos(const char *toStem, size_t length) {
  sb_stemmer_stem(stemmer_, (const sb_symbol*) toStem, length);
  return sb_stemmer_length(stemmer_);
}

Utf8Stemmer::~Utf8Stemmer() {
  sb_stemmer_delete(stemmer_);
}

} // text_util
} // relevanced
