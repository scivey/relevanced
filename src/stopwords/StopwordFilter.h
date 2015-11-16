#pragma once
#include <string>

namespace relevanced {
namespace stopwords {

// most of the real junk (numbers, punctuation, etc)
// is filtered out prior to StopwordFilter being used,
// during the token iteration step.
// by the time StopwordFilter enters the scene, we
// assume that we're just trying to filter out
// *actual words* that are uninformative.

class StopwordFilterIf {
 public:
  virtual bool isStopword(const std::string &stemmedWord) = 0;
};

class StopwordFilter : public StopwordFilterIf {
 public:
  bool isStopword(const std::string &stemmedWord) override;
};

} // stopwords
} // relevanced
