#pragma once
#include <string>
#include <cstring>


namespace relevanced {
namespace text_util {

struct StringView {
  const char *base;
  size_t len;
  StringView(const char *base, size_t len): base(base), len(len){}
};

bool operator==(const StringView &sv1, const StringView &sv2) {
  return strcmp(sv1.base, sv2.base) == 0;
}

} // text_util
} // relevanced

namespace std {
  template<>
  struct less<relevanced::text_util::StringView> {
    bool operator()(const relevanced::text_util::StringView &s1, const relevanced::text_util::StringView &s2) {
      return strncmp(s1.base, s2.base, s1.len) < 0;
    }
  };

  template<>
  struct hash<relevanced::text_util::StringView> {
    size_t operator()(const relevanced::text_util::StringView &sv) const {
      return std::hash<const char*>()(sv.base);
    }
  };
}
