#include <memory>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "state.grpc.pb.h"

class StateClient {
 public:
  StateClient(std::shared_ptr<grpc::Channel> channel)
      : stub(State::NewStub(channel)) { }

  void speed_limit_update(int speed) {
    grpc::ClientContext context;
    SpeedLimitUpdate update;
    update.set_time(0);
    update.set_speed_limit((float) speed);
    Status status;
    grpc::Status status2 = stub->speed_limit_update(&context, update, &status);
    if (!status2.ok()) {
      std::cerr << "GetFeature rpc failed." << std::endl;
    }
  }

private:
  std::unique_ptr<State::Stub> stub;
};
