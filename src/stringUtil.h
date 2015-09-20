#pragma once
#include <string>
#include <sstream>

namespace {
  using namespace std;
}

namespace stringUtil {

bool isOnlyAscii(const string &text) {
  for (size_t i = 0; i < text.size(); i++) {
    unsigned char c = text.at(i);
    if (c > 127) {
      return false;
    }
  }
  return true;
}

} // stringUtil