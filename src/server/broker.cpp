#include "../libs/zhelpers.hpp"

int main (void) 
{
    //  Prepare our context and sockets
    zmq::context_t context(1);
    zmq::socket_t frontend(context, ZMQ_ROUTER);
    zmq::socket_t backend(context, ZMQ_ROUTER);
    zmq::socket_t workerChannel(context, ZMQ_REP);
    zmq_bind (frontend, "tcp://*:5559");
    zmq_bind (backend,  "tcp://*:5560");
    zmq_bind (workerChannel, "tcp://*:5561");

    // Store worker information
    // first is worker id from zmq, second is hash of worker
    std::unordered_map<std::string, std::string> workers;

    //  Initialize poll set
    zmq_pollitem_t items [] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend,  0, ZMQ_POLLIN, 0 },
        { workerChannel, 0, ZMQ_POLLIN, 0 }
    };
    //  Switch messages between sockets
    while (1) {
        zmq_msg_t message;
        zmq::poll (items, 3, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            zmq::message_t client_id_msg;
            frontend.recv(client_id_msg, zmq::recv_flags::none);
            std::cout << "Received client id: " << client_id_msg.to_string() << std::endl;

            zmq::message_t empty_msg;
            frontend.recv(empty_msg, zmq::recv_flags::none);
            std::cout << "Received empty message: " << empty_msg.to_string() << std::endl;

            zmq::message_t request_msg;
            frontend.recv(request_msg, zmq::recv_flags::none);
            std::cout << "Received request: " << request_msg.to_string() << std::endl;

            // Select worker based on custom load balancing logic
            if (!workers.empty()) {
                auto selected_worker = workers.begin();

                // Forward the request to the selected worker
                std::cout << "Forwarding request to worker: " << selected_worker->first << std::endl;
                backend.send(zmq::buffer(selected_worker->first), zmq::send_flags::sndmore);
                backend.send(zmq::message_t{}, zmq::send_flags::sndmore);
                backend.send(client_id_msg, zmq::send_flags::sndmore);
                backend.send(request_msg, zmq::send_flags::none);
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            zmq::message_t client_id;
            backend.recv(client_id, zmq::recv_flags::none);
            std::cout << "Received client id: " << client_id.to_string() << std::endl;

            zmq::message_t request_msg;
            backend.recv(request_msg, zmq::recv_flags::none);
            std::cout << "Received request: " << request_msg.to_string() << std::endl;

            // Build a reply message
            frontend.send(client_id, zmq::send_flags::sndmore);
            frontend.send(zmq::message_t{}, zmq::send_flags::sndmore);
            frontend.send(request_msg, zmq::send_flags::none);
        }
        if (items [2].revents & ZMQ_POLLIN) {
            std::string worker_id = s_recv(workerChannel);
            std::cout << "Received worker id: " << worker_id << std::endl;

            // If worker is not in the list, add it
            if (workers.find(worker_id) == workers.end()) {
                std::cout << "Adding worker: " << worker_id << std::endl;
                workers[worker_id] = "hash";
                workerChannel.send(zmq::str_buffer("OK"), zmq::send_flags::none);
            }
            else{
                workerChannel.send(zmq::str_buffer("NOK"), zmq::send_flags::none);
            }
        }
    }
    return 0;
}
