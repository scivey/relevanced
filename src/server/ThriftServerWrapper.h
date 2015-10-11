#pragma once

#include <memory>

namespace apache {
namespace thrift {
class ThriftServer;
}
}

namespace relevanced {
namespace server {

class ThriftServerWrapper {
 protected:
  std::shared_ptr<apache::thrift::ThriftServer> pImpl_;

 public:
  ThriftServerWrapper(std::shared_ptr<apache::thrift::ThriftServer>);
  void serve();
};
}
}