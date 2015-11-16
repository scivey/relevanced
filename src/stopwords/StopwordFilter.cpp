#include <string>
#include <set>
#include "stopwords/english_stopwords.h"
#include "stopwords/german_stopwords.h"

#include "stopwords/StopwordFilter.h"

using namespace std;

namespace relevanced {
namespace stopwords {

bool StopwordFilter::isStopword(const string &word) {

  if (word.size() <= 2) {
    return true;
  }
  return isEnglishStopword(word);
}

} // stopwords
} // relevanced
