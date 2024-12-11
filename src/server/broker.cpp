#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include "consistentHashing.hpp"
#include <filesystem>
#include <unordered_map>
#include <iostream>

namespace fs = std::filesystem;

ConsistentHashRing workerHashRing;

const int VIRTUAL_NODES = 3;  // Number of virtual nodes per worker
const int REPLICATION_FACTOR = 3; // Number of replicas for each key

void receive_empty(zmq::socket_t& socket) {
    zmq::message_t empty_msg;
    socket.recv(empty_msg, zmq::recv_flags::none);
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

void printPreferenceList(const std::vector<Node*>& preferenceList) {
    std::cout << "Preference list for the request:" << std::endl;
    for (size_t i = 0; i < preferenceList.size(); ++i) {
        std::cout << "  Node " << i + 1 << ": " << preferenceList[i]->id << std::endl;
    }
    std::cout << std::endl;
}

void printVirtualNodes() {
    std::cout << "Current virtual nodes in the hash ring:" << std::endl;
    for (const auto& [hash, node] : workerHashRing.getRing()) {
        std::cout << "  Hash: " << hash << " -> Node: " << node->id << std::endl;
    }
    std::cout << std::endl;
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

                std::vector<Node*> preferenceList;

                json j = json::parse(request);
                std::string list_id = j["id"];

                int workerCount = workerHashRing.getRing().size() / VIRTUAL_NODES;

                std::cout << "Worker count: " << workerCount << std::endl;

                // Use consistent hashing to forward the request
                if(workerCount < REPLICATION_FACTOR){   
                    std::cout << "Worker count is less than replication factor. Forwarding request to all workers." << workerCount << std::endl;       
                    preferenceList = workerHashRing.getPreferenceList(list_id, workerCount);
                }
                else {
                    std::cout << "Worker count is greater than or equal to replication factor. Forwarding request to " << REPLICATION_FACTOR << " workers." << std::endl;
                    preferenceList = workerHashRing.getPreferenceList(list_id, REPLICATION_FACTOR);
                }

                printPreferenceList(preferenceList);

                // Forward the request to all workers in the preference list
                for (Node* node : preferenceList) {
                    std::string worker_id = node->id;
                    // std::cout << "Forwarding request to worker: " << worker_id << std::endl;

                    s_sendmore(backend, worker_id);
                    s_sendmore(backend, std::string(""));
                    s_sendmore(backend, client_addr);
                    s_sendmore(backend, std::string(""));
                    s_send(backend, request);
                }
            } else {
                std::cerr << "No workers available to handle request." << std::endl;
            }

            sendPub(request, publisher);
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
                workerHashRing.addNode(workerNode, VIRTUAL_NODES);

                // Initialize directories for the worker
                initializeWorkerDirectory(worker_id);

                printVirtualNodes();

                std::cout << "Worker registered and directories initialized: " << worker_id << std::endl;
            } else {
                // Handle worker reply
                receive_empty(backend);
                std::string reply = s_recv(backend);
                std::cout << "Received reply from worker: " << worker_id << std::endl;

                s_sendmore(frontend, client_addr);
                s_sendmore(frontend, std::string(""));
                s_send(frontend, reply);
            }
        }
    }
    return 0;
}