#include <string>
#include <mitie/stemmer.h>
#include "stemmer/PorterStemmer.h"
#include "util/util.h"
using namespace std;

namespace relevanced {
namespace stemmer {

void PorterStemmer::stemInPlace(string &word) {
  word = stem(word);
}

string PorterStemmer::stem(const string &word) {
  return mitie::stem_word(word);
}

} // stemmer
} // relevanced
