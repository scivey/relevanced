#include <string>
#include <boost/algorithm/string/case_conv.hpp>

#include "PorterStemmer.h"
#include "porter_stemmer.h"
#include "stringUtil.h"
using namespace std;

namespace stemmer {

void PorterStemmer::stemInPlace(string &word) {
  if (stringUtil::isOnlyAscii(word)) {
    boost::algorithm::to_lower(word);
  }
  int stemEnd = porter_stem((char*) word.c_str(), 0, word.size());
  word.erase(stemEnd);
}

string PorterStemmer::stem(const string &word) {
  string result = word;
  stemInPlace(result);
  return result;
}

} // stemmer