#if defined(WIN32) && !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0A00
#endif
#include "grpc++/channel.h"
#include "grpc++/grpc++.h"
#include "gtest/gtest.h"

namespace Envoy {
namespace Grpc {
namespace {

// Validate we can include/link some basic Google gRPC C++ library objects.
TEST(GoogleGrpc, All) {
  std::shared_ptr<grpc::ChannelCredentials> creds = grpc::InsecureChannelCredentials();
  CreateChannel("1.2.3.4:5678", creds);
}

} // namespace
} // namespace Grpc
} // namespace Envoy
