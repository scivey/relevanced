#pragma once
#include <string>
#include <cstring>
#include <folly/Hash.h>

namespace relevanced {
namespace text_util {

struct StringView {
  const char *base;
  size_t len;
  StringView(const char *base, size_t len): base(base), len(len){}
};

bool operator==(const StringView &sv1, const StringView &sv2);

} // text_util
} // relevanced

namespace std {
  template<>
  struct less<relevanced::text_util::StringView> {
    bool operator()(const relevanced::text_util::StringView &s1, const relevanced::text_util::StringView &s2) {
      return strncmp(s1.base, s2.base, s1.len - 1) < 0;
    }
  };

  template<>
  struct hash<relevanced::text_util::StringView> {
    size_t operator()(const relevanced::text_util::StringView &sv) const {
      return folly::hash::fnv64_buf((void*) sv.base, sv.len);
    }
  };
}
