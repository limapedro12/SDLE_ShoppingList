#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    // Initialize ZeroMQ context and socket
    zmq::context_t context(1);                     // 1 thread in the context
    zmq::socket_t socket(context, ZMQ_REQ);     // Create a request socket

    socket.connect("tcp://localhost:5559");

    // create list
    Message creation("create", "1", {});
    s_send(socket, creation.toString());

    std::string reply_str = s_recv(socket);

    std::cout << "Received reply :" << " [" << reply_str << "]" << std::endl;

    for (int request_nbr = 0; request_nbr < 10; ++request_nbr) {
        // Create the message object
        //std::unordered_map<std::string, int> data = {{"a", 1}, {"b", 2}, {"c", 3}};
        //Message message("helloWorld", "mrBombastic2", data);
        ShoppingList shopping_list("1");
        shopping_list.set_user_id(1);
        shopping_list.add("apple");
        shopping_list.add("banana");
        shopping_list.add("apple", request_nbr);

        shopping_list.fresh();
        shopping_list.add("apple", 2);
        Message message(shopping_list, "merge");


        // Serialize the message into a string and send it
        s_send(socket, message.toString());

        // Receive the reply
        std::string reply_str = s_recv(socket);

        // Print the received reply
        std::cout << "Received reply " << request_nbr << " [" << reply_str << "]" << std::endl;
    }
    return 0;
}

