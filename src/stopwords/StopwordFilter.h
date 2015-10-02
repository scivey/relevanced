#pragma once
#include <string>

namespace relevanced {
namespace stopwords {

class StopwordFilterIf {
public:
  virtual bool isStopword(const std::string &stemmedWord) = 0;
};

class StopwordFilter: public StopwordFilterIf {
public:
  bool isStopword(const std::string &stemmedWord) override;
};

} // stopwords
} // relevanced

