#include <cstring>
#include "text_util/StringView.h"
namespace relevanced {
namespace text_util {

bool operator==(const StringView &sv1, const StringView &sv2) {
  return strncmp(sv1.base, sv2.base, sv1.len - 1) == 0;
}

}
}