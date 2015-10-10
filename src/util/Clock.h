#pragma once
#include "util.h"

namespace relevanced {
namespace util {

class ClockIf {
public:
  virtual int64_t getEpochTime() = 0;
};

class Clock: public ClockIf {
public:
  int64_t getEpochTime() override {
    return getChronoEpochTime();
  }
};


} // util
} // relevanced

