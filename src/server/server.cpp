#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <string>
#include <iostream>

/**
 * @brief Handle the received message
 */
Message handleMessage(Message received){
    switch (received.getOperation())
    {
        case "helloWorld":
            std::unordered_map<std::string, int> data = {{"Here", 1}, {"Is", 2}, {"Data", 3}};
            return Message(received.getOperation(), received.getId(), data);
            break;
        default:
            std::cout << "Error: Operation requested not handleable" << std::endl;
            std::unordered_map<std::string, int> data = {{"Error", 1}};
            return Message("error", received.getId(), data);
            break;
    }
}

int main (void) 
{
    zmq::context_t context(1);

    zmq::socket_t socket(context, ZMQ_REP);
    std::cout << "Binding to port 5560..." << std::endl;
    socket.connect("tcp://localhost:5560");
    std::cout << "Connected!" << std::endl;

    while (1) {
        std::cout << "Waiting for request..." << std::endl;
        //  Wait for next request from client
        std::string string = s_recv (socket);
        std::cout << "Received request: [" << string << "]" << std::endl;

        // Parse received message
        Message received(string);

        //  Do some 'work'
        sleep (1);

        // Build a reply message
        Message reply = handleMessage(received);

        //  Send reply back to client
        s_send (socket, reply.toString());
    }
    return 0;
}
