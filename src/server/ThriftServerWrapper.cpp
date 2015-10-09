#include <memory>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include "server/ThriftServerWrapper.h"

using namespace std;
using apache::thrift::ThriftServer;

namespace relevanced {
namespace server {

ThriftServerWrapper::ThriftServerWrapper(shared_ptr<ThriftServer> pImpl):
  pImpl_(pImpl){}

void ThriftServerWrapper::serve() {
  pImpl_->serve();
}

} // server
} // relevanced
