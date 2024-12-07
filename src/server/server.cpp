#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include "../crdt/headers/ShoppingList.hpp"
#include <string>
#include <iostream>
#include "handler.hpp"
#include "consistentHashing.hpp"

ConsistentHashRing hashRing;

// Initialize nodes for the ring
void initializeNodes() {
    Node* node1 = new Node("Node1");
    Node* node2 = new Node("Node2");
    Node* node3 = new Node("Node3");

    hashRing.addNode(node1);
    hashRing.addNode(node2);
    hashRing.addNode(node3);

    std::cout << "Nodes added to the ring: Node1, Node2, Node3" << std::endl;
}

/**
 * @brief Handle the received message
 */
Message handleMessage(nlohmann::json received){
    std::unordered_map<std::string, int> data = {};
    std::string operation = received["operation"]; // its worth looking into an enum (do those even exist in c++?)
    std::string id = received["id"];
    if (operation == "helloWorld"){
        data = {{"Here", 1}, {"Is", 2}, {"Data", 3}};
        return Message(operation, id, data);
    }
    else if (operation == "create"){
        //createShoppingList(received);
        data = {{"Success", 1}};
        return Message("create", id, data);
    }
    else if (operation == "get"){
        json rep = getShoppingList(received);
        if (rep.empty()){
            data = {{"Error", 1}};
            return Message("error", id, data);
        }
        data = {{"Success", 1}, {"data", rep["data"]}};
        return Message("get", id, data);
    }
    else if (operation == "erase"){
        eraseShoppingList(received);
        data = {{"Success", 1}};
        return Message("erase", id, data);
    }
    else if (operation == "merge"){
        mergeShoppingList(received);
        data = {{"Success", 1}};
        return Message("merge", id, data);
    } 
    else {
        std::cout << "Error: Operation requested not handleable" << std::endl;
        data = {{"Error", 1}};
        return Message("error", id, data);
    }
}

int main (void) 
{
    initializeNodes();

    zmq::context_t context(1);

    std::srand(std::time(0));
    std::string workerID = "worker"+std::to_string(std::rand());

    zmq::socket_t workerChannel(context, ZMQ_REQ);
    std::cout << "Worker ID: " << workerID << std::endl;
    std::cout << "Worker ID set" << std::endl;
    workerChannel.connect("tcp://localhost:5561");
    std::cout << "Worker connected" << std::endl;

    std::cout << "Sending message: [" << workerID << "]" << std::endl;
    s_send(workerChannel, workerID);
    std::cout << "Message sent" << std::endl;

    std::string reply = s_recv(workerChannel);
    std::cout << "Received reply: [" << reply << "]" << std::endl;
    if (reply == "OK") {
        std::cout << "Worker connected" << std::endl;
    }
    else {
        std::cout << "Couldn't connect worker" << std::endl;
        return 1;
    }

    zmq::socket_t socket(context, ZMQ_DEALER);
    socket.connect("tcp://localhost:5560");

    while (1) {
        //  Wait for next request from client
        zmq::message_t worker_id;
        socket.recv(worker_id, zmq::recv_flags::none);
        std::cout << "Received worker id: " << worker_id.to_string() << std::endl;

        zmq::message_t empty_msg;
        socket.recv(empty_msg, zmq::recv_flags::none);
        std::cout << "Received empty message: " << empty_msg.to_string() << std::endl;

        zmq::message_t client_id;
        socket.recv(client_id, zmq::recv_flags::none);
        std::cout << "Received client id: " << client_id.to_string() << std::endl;
        
        zmq::message_t request_msg;
        socket.recv(request_msg, zmq::recv_flags::none);
        std::cout << "Received request: " << request_msg.to_string() << std::endl;

        //std::string received = s_recv (socket);
        //std::cout << "Received request: [" << received << "]" << std::endl << std::endl;

        // Parse received message
        //Message received(string);
        nlohmann::json json = nlohmann::json::parse(request_msg.to_string());

        // Build a reply message
        Message reply = handleMessage(json);
        std::cout << "Reply: " << reply.toString() << std::endl;

        //  Send reply back to client
        std::cout << "Sending reply" << std::endl;
        socket.send(worker_id, zmq::send_flags::sndmore);
        std::cout << "Sent client id" << std::endl;
        socket.send(zmq::message_t{}, zmq::send_flags::sndmore);
        std::cout << "Sent empty message" << std::endl;
        socket.send(zmq::buffer(reply.toString()), zmq::send_flags::none);
        std::cout << "Sent reply" << std::endl;
    }
    return 0;
}
