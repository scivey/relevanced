#include <string>
#include <set>
#include "stopwords/english_stopwords.h"
#include "stopwords/french_stopwords.h"
#include "stopwords/german_stopwords.h"
#include "stopwords/italian_stopwords.h"
#include "stopwords/russian_stopwords.h"
#include "stopwords/spanish_stopwords.h"
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
