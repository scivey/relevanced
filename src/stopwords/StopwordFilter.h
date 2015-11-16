#pragma once
#include <string>
#include "gen-cpp2/RelevancedProtocol_types.h"
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
  virtual bool isStopword(
    const std::string &stemmedWord,
    thrift_protocol::Language language
  ) = 0;
};

class StopwordFilter : public StopwordFilterIf {
 public:
  bool isStopword(
    const std::string &stemmedWord,
    thrift_protocol::Language language
  ) override;
};

} // stopwords
} // relevanced
