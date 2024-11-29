#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include "../crdt/headers/ShoppingList.hpp"
#include <string>
#include <iostream>
#include "handler.hpp"

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
        createShoppingList(received);
        data = {{"Success", 1}};
        return Message("create", id, data);
    }
    else if (operation == "get"){
        getShoppingList(received);
        data = {{"Success", 1}};
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
    zmq::context_t context(1);

    zmq::socket_t socket(context, ZMQ_REP);
    socket.connect("tcp://localhost:5560");

    while (1) {
        //  Wait for next request from client
        std::string received = s_recv (socket);
        std::cout << "Received request: [" << received << "]" << std::endl << std::endl;

        // Parse received message
        //Message received(string);
        nlohmann::json json = nlohmann::json::parse(received);

        ShoppingList shoppingList(json["id"], json["data"]);
        cout << "Shopping list: " << shoppingList.print() << endl;

        // Build a reply message
        Message reply = handleMessage(json);

        //  Send reply back to client
        s_send (socket, reply.toString());
    }
    return 0;
}
