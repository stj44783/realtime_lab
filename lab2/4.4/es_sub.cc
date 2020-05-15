#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#include <grpcpp/grpcpp.h>

#include "es.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;

using es::TopicRequest;
using es::EventService;
using es::TopicData;

class Subscriber {
 public:
  Subscriber(int pubID, std::shared_ptr<Channel> channel)
      : id_(pubID),
        stub_(EventService::NewStub(channel)) {}

  void Subscribe(TopicRequest request) {
    ClientContext context;
    TopicData td;
    std::unique_ptr<ClientReader<TopicData> > reader(
        stub_->Subscribe(&context, request));
    std::cout << "Start to receive data...\n";
    while (reader->Read(&td)) {
      std::cout << "{" << td.topic() << ": " << td.data() << "}  ";
    }
    Status status = reader->Finish();
    if(status.ok()){
	std::cout << "connect.\n";
    }else{
	    std::cout << "Failed.\n";
    }
  }

 private:
  std::unique_ptr<EventService::Stub> stub_;
  int id_;
};

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " -t topic\n";
    exit(0);
  }
  std::string target_str;
  target_str = "localhost:50051";
  Subscriber sub(2, grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  TopicRequest request;
  request.set_topic(argv[2]);
  sub.Subscribe(request);
  sleep(3600);

  return 0;
}
