#include "../libs/zhelpers.hpp"

int receiveSubscriptions(zmq::socket_t &subscriber){
    while (1) {
      //  Read envelope with address
      std::string address = s_recv (subscriber);
      //  Read message contents
      std::string contents = s_recv (subscriber);
		
      std::cout << "[" << address << "] " << contents << std::endl;
    }
    return 0;
}

int main(void) {
  zmq::context_t context(1);
  zmq::socket_t subscriber (context, ZMQ_SUB);
  subscriber.connect("tcp://localhost:5561");
  subscriber.set( zmq::sockopt::subscribe, "7b9c202803a812109ab060e3ea7eb41a");
  receiveSubscriptions(subscriber);
  return 0;
}
