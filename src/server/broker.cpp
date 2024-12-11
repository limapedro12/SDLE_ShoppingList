#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include "consistentHashing.hpp"
#include <filesystem>
#include <unordered_map>
#include <iostream>

namespace fs = std::filesystem;

ConsistentHashRing workerHashRing;

void receive_empty(zmq::socket_t& socket) {
    zmq::message_t empty_msg;
    zmq::recv_result_t result = socket.recv(empty_msg, zmq::recv_flags::none);
    assert(empty_msg.size() == 0);
}

void initializeWorkerDirectory(const std::string& worker_id) {
    std::string base_path = "nodes/" + worker_id;
    std::string list_path = base_path + "/lists";

    try {
        if (!fs::exists(base_path)) {
            fs::create_directories(base_path);
            fs::create_directories(list_path);
            std::cout << "Created directories for worker: " << worker_id << std::endl;
        } else {
            std::cout << "Directories already exist for worker: " << worker_id << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating directories for worker " << worker_id << ": " << e.what() << std::endl;
    }
}

void sendPub(const std::string &message, zmq::socket_t &publisher){
    json json_message = json::parse(message);
    std::string operation = json_message["operation"];
    std::string list_id = json_message["id"];

    if (operation == "merge"){
        s_sendmore(publisher, list_id);
        s_send(publisher, message);
    }
}

int main(void) {
    // Prepare context and sockets
    zmq::context_t context(1);
    zmq::socket_t frontend(context, ZMQ_ROUTER);
    zmq::socket_t backend(context, ZMQ_ROUTER);
    zmq::socket_t publisher(context, ZMQ_PUB);
    zmq_bind(frontend, "tcp://*:5559");
    zmq_bind(backend, "tcp://*:5560");
    zmq_bind(publisher, "tcp://*:5561");

    zmq_pollitem_t items[] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend,  0, ZMQ_POLLIN, 0 }
    };

    while (true) {
        zmq::poll(items, 2, -1);

        if (items[0].revents & ZMQ_POLLIN) {
            // Handle client requests
            std::string client_addr = s_recv(frontend);
            receive_empty(frontend);

            std::string request = s_recv(frontend);
            std::cout << "Received request from client: " << client_addr << std::endl;

            if (!workerHashRing.isEmpty()) {
                // Use consistent hashing to forward the request
                std::string worker_id = workerHashRing.getNode(client_addr)->id;
                std::cout << "Forwarding request to worker: " << worker_id << std::endl;

                s_sendmore(backend, worker_id);
                s_sendmore(backend, std::string(""));
                s_sendmore(backend, client_addr);
                s_sendmore(backend, std::string(""));
                s_send(backend, request);
            } else {
                std::cerr << "No workers available to handle request." << std::endl;
                s_sendmore(frontend, client_addr);
                s_sendmore(frontend, std::string(""));
                std::string reply = "{'No workers available to handle request.': 1}";
                s_send(frontend, reply);
                
                sendPub(request, publisher);
            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            // Handle worker messages
            std::string worker_id = s_recv(backend);
            receive_empty(backend);

            std::string client_addr = s_recv(backend);

            if (client_addr == "READY") {
                // Register the worker
                std::cout << "Registering worker: " << worker_id << std::endl;

                // Add to consistent hash ring
                Node* workerNode = new Node(worker_id);
                workerHashRing.addNode(workerNode);

                // Initialize directories for the worker
                initializeWorkerDirectory(worker_id);

                std::cout << "Worker registered and directories initialized: " << worker_id << std::endl;
            } else {
                // Handle worker reply

                cout << "Received reply from worker: " << worker_id << std::endl;
                receive_empty(backend);
                std::string reply = s_recv(backend);
                std::cout << "Received reply from worker: " << worker_id << std::endl;

                // Forward the reply to the client
                std::cout << "Forwarding reply to client: " << client_addr << std::endl;

                s_sendmore(frontend, client_addr);
                s_sendmore(frontend, std::string(""));
                s_send(frontend, reply);

                sendPub(reply, publisher);
            }
        }
    }
    return 0;
}