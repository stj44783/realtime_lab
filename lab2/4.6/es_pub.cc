#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "es.grpc.pb.h"
#include <sys/time.h>

//#include <google/protobuf/util/time_util.h>


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

using es::TopicRequest;
using es::EventService;
using es::TopicData;
using es::NoUse;

using google::protobuf::Timestamp;

class Publisher {
 public:
  Publisher(int pubID, std::shared_ptr<Channel> channel)
      : id_(pubID),
        stub_(EventService::NewStub(channel)), 
        writer_(stub_->Publish(&context_, &nouse_)) {}

  void Publish(const TopicData td) {
    if (!writer_->Write(td)) {
      // Broken stream.
      std::cout << "rpc failed: broken stream." << std::endl;
      writer_->WritesDone();
      Status status = writer_->Finish();
      if (status.ok()) {
        std::cout << "Finished.\n";
      } else {
        std::cout << "RecordRoute rpc failed." << std::endl;
      }
      exit(0);
    }
    else {
      std::cout << "sent {" << td.topic() << ": " << td.data() << "}" << std::endl;
    }
  }

  void done() {
    writer_->WritesDone();
    Status status = writer_->Finish();
    if (status.ok()) {
      std::cout << "Finished.\n";
    } else {
      std::cout << "RecordRoute rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<EventService::Stub> stub_;
  int id_;
  ClientContext context_;
  NoUse nouse_;
  std::unique_ptr<ClientWriter<TopicData> > writer_;
};

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "Usage: " << argv[0] << " -t topic -m message\n";
    exit(0);
  }
  std::string target_str;
  target_str = "localhost:50051";
  Publisher pub(1, grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  TopicData td;
  std::cout << "Start to send data to our server..\n";
  td.set_topic(argv[2]);
  td.set_data(argv[4]);
  struct timeval tv;
  gettimeofday(&tv, NULL);
  Timestamp *tp = td.mutable_timestamp();
  tp->set_seconds(tv.tv_sec);
  tp->set_nanos(tv.tv_usec * 1000);
  pub.Publish(td);
  pub.done();

  return 0;
}