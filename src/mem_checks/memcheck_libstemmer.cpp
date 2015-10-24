#include <string>
#include <cstring>
#include "libstemmer.h"

void cycle() {
  struct sb_stemmer *instance;
  instance = sb_stemmer_new("en", "UTF_8");
  std::string text {"embarassing"};
  sb_stemmer_stem(instance, (const sb_symbol*) text.c_str(), text.size());
  sb_stemmer_delete(instance);
}

int main() {
  for (size_t i = 0; i < 1000; i++) {
    cycle();
  }
}

