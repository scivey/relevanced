#include <string>
#include <cstring>
#include "text_util/Utf8Stemmer.h"
#include "libstemmer.h"

namespace relevanced {
namespace text_util {

Utf8Stemmer::Utf8Stemmer() {
  stemmer_ = sb_stemmer_new("en", "UTF_8");
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
