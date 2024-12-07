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

    srand(time(NULL));

    zmq::socket_t socket(context, ZMQ_REQ);
    std::string zmqID = s_set_id(socket);
    socket.connect("tcp://localhost:5560");

    s_send(socket, std::string("READY"));

    /*
    if (s_recv(socket)!= "OK"){
        std::cout << "Failed to connect to broker" << std::endl;
        return 1;
    }
    */

    std::cout << "Connected to broker" << std::endl;

    while (1) {
        //  Wait for next request from client
        std::string address = s_recv(socket);
        std::cout << "Received address: [" << address << "]" << std::endl;

        //  Get the empty message
        std::string empty = s_recv(socket);
        std::cout << "Received empty message: " << empty << std::endl;

        //  Get the request message
        std::string request_msg = s_recv(socket);
        std::cout << "Received request: [" << request_msg << "]" << std::endl;

        // Parse received request
        //Message received(string);
        nlohmann::json json = nlohmann::json::parse(request_msg);

        // Build a reply message
        Message reply = handleMessage(json);
        std::cout << "Reply: " << reply.toString() << std::endl;

        //  Send reply back to client
        std::cout << "Sending reply" << std::endl;
        s_sendmore(socket, address);
        s_sendmore(socket, std::string(""));
        s_send(socket, reply.toString());
    }
    return 0;
}
