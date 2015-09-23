#pragma once

#pragma once
#include <string>

class Document {
public:
  const std::string id;
  const std::string text;
  Document(const std::string &idStr, const std::string &textStr): id(idStr), text(textStr) {}
};
