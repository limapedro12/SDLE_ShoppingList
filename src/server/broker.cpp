#include "../libs/zhelpers.hpp"

void receive_empty(zmq::socket_t& socket){
    zmq::message_t empty_msg;
    socket.recv(empty_msg, zmq::recv_flags::none);
    assert(empty_msg.size() == 0);
    std::cout << "Received empty message: " << empty_msg.to_string() << std::endl;
}

int main (void) 
{
    //  Prepare our context and sockets
    zmq::context_t context(1);
    zmq::socket_t frontend(context, ZMQ_ROUTER);
    zmq::socket_t backend(context, ZMQ_ROUTER);
    zmq_bind (frontend, "tcp://*:5559");
    zmq_bind (backend,  "tcp://*:5560");

    // Store worker information
    // first is worker id from zmq, second is hash of worker
    std::unordered_map<std::string, std::string> workers;

    //  Initialize poll set
    zmq_pollitem_t items [] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend,  0, ZMQ_POLLIN, 0 }
    };
    //  Switch messages between sockets
    while (1) {
        zmq_msg_t message;
        zmq::poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            std::string client_addr = s_recv(frontend);
            std::cout << "Received client address: " << client_addr << std::endl;

            receive_empty(frontend);

            std::string request = s_recv(frontend);
            std::cout << "Received request: " << request << std::endl;

            if (!workers.empty()){
                std::string worker_id = workers.begin()->first; // replace with hashing logic
                std::cout << "Sending request to worker: " << worker_id << std::endl;
                s_sendmore(backend, worker_id);
                s_sendmore(backend, std::string(""));
                s_sendmore(backend, client_addr);
                s_sendmore(backend, std::string(""));
                s_send(backend, request);
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            std::string worker_id = s_recv(backend);
            std::cout << "Received worker id: " << worker_id << std::endl;

            receive_empty(backend);
            
            std::string client_addr = s_recv(backend);
            std::cout << "Received client address: " << client_addr << std::endl;

            if (client_addr.compare("READY") != 0){
                receive_empty(backend);
                std::string reply = s_recv(backend);
                std::cout << "Received reply: " << reply << std::endl;
                s_sendmore(frontend, client_addr);
                s_sendmore(frontend, std::string(""));
                s_send(frontend, reply);
            }
            else{
                /*
                s_sendmore(backend, worker_id);
                s_sendmore(backend, std::string(""));
                s_send(backend, std::string("OK"));
                */
                workers[worker_id] = "hash";
            }
        }
    }
    return 0;
}
