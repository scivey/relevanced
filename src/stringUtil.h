#pragma once
#include <string>
#include <sstream>

namespace {
  using namespace std;
}

namespace stringUtil {

string lowerCase(const string &text) {
  ostringstream oss;
  size_t capitalOffset = 25;
  for(size_t i = 0; i < text.size(); i++) {
    char c = text.at(i);
    if (c >= 97 && c <= 122) {
      oss << (c - capitalOffset);
    } else {
      oss << c;
    }
  }
  return oss.str();
}

string upperCase(const string &text) {
  ostringstream oss;
  size_t capitalOffset = 25;
  for(size_t i = 0; i < text.size(); i++) {
    char c = text.at(i);
    if (c >= 65 && c <= 90) {
      oss << (c + capitalOffset);
    } else {
      oss << c;
    }
  }
  return oss.str();
}

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