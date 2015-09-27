#include <string>
#include <mitie/stemmer.h>
#include "PorterStemmer.h"
#include "stringUtil.h"
using namespace std;

namespace stemmer {

void PorterStemmer::stemInPlace(string &word) {
  word = stem(word);
}

string PorterStemmer::stem(const string &word) {
  return mitie::stem_word(word);
}

} // stemmer