#pragma once

#include <string>
#include "gen-cpp2/RelevancedProtocol_types.h"

namespace relevanced {
namespace models {

class Document {
 public:
  const std::string id;
  std::string text;
  thrift_protocol::Language language;

  Document(const std::string &idStr,
    const std::string &textStr,
    thrift_protocol::Language language
  ) : id(idStr), text(textStr), language(language) {}

};

} // models
} // relevanced
