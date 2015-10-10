#pragma once
#include "util.h"

#include <string>
#include <memory>

namespace relevanced {
namespace util {

class Sha1HasherIf {
public:
  virtual std::string hash(const std::string&) = 0;
  virtual std::string hash(std::shared_ptr<std::string>) = 0;
};

class Sha1Hasher: public Sha1HasherIf {
public:
  std::string hash(const std::string& text) override {
    return sha1(text);
  }
  std::string hash(std::shared_ptr<std::string> textPtr) override {
    auto text = *textPtr;
    return sha1(text);
  }
};

}
}