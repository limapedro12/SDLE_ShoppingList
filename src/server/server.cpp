#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <string>
#include <iostream>

/**
 * @brief Handle the received message
 */
Message handleMessage(Message received){
    std::unordered_map<std::string, int> data = {};
    std::string operation = received.getOperation(); // its worth looking into an enum (do those even exist in c++?)º
    if (operation == "helloWorld"){
        // the json gets parsed into a map, but you can also just use the json object as a map, this may be more efficient however
        for (auto const& x : received.getData())
        {
            std::cout << x.first  // string (key)
                      << ':'
                      << x.second // string's value 
                      << std::endl ;
        }
        data = {{"Here", 1}, {"Is", 2}, {"Data", 3}};
        return Message(received.getOperation(), received.getId(), data);
    } else {
        std::cout << "Error: Operation requested not handleable" << std::endl;
        data = {{"Error", 1}};
        return Message("error", received.getId(), data);
    }
}

int main (void) 
{
    zmq::context_t context(1);

    zmq::socket_t socket(context, ZMQ_REP);
    socket.connect("tcp://localhost:5560");

    while (1) {
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
