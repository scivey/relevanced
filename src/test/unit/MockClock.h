#pragma once
#include "gmock/gmock.h"
#include "util/Clock.h"

class MockClock: public relevanced::util::ClockIf {
public:
  MOCK_METHOD0(getEpochTime, int64_t());
};
