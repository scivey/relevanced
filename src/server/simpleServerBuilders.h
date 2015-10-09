#pragma once

#include <memory>
#include "server/RelevanceServerOptions.h"
#include "server/ThriftServerWrapper.h"


namespace apache {
  namespace thrift {
    class ThriftServer;
  }
}

namespace relevanced {
namespace server {
  std::shared_ptr<ThriftServerWrapper> buildNormalThriftServer(std::shared_ptr<RelevanceServerOptions>);
}
}
