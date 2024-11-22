#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <iostream>
#include <string>
#include <vector>

/*
int main (void) 
{
    void *context = zmq_ctx_new ();

    //  Socket to talk to server
    zmq::socket_t *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5559");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        std::unordered_map <std::string, int> data = {{"a", 1}, {"b", 2}, {"c", 3}};
        Message message = Message("helloWorld", "mrBombastic2", data);
        s_send (requester, message.toString());
        char *string = s_recv (requester);
        printf ("Received reply %d [%s]\n", request_nbr, string);
        free (string);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
*/

int main() {
    // Initialize ZeroMQ context and socket
    zmq::context_t context(1);                     // 1 thread in the context
    zmq::socket_t socket(context, ZMQ_REQ);     // Create a request socket

    socket.connect("tcp://localhost:5559");

    for (int request_nbr = 0; request_nbr < 10; ++request_nbr) {
        // Create the message object
        std::unordered_map<std::string, int> data = {{"a", 1}, {"b", 2}, {"c", 3}};
        Message message("helloWorld", "mrBombastic2", data);

        // Serialize the message into a string and send it
        s_send(socket, message.toString());

        // Receive the reply
        std::string reply_str = s_recv(socket);

        // Print the received reply
        std::cout << "Received reply " << request_nbr << " [" << reply_str << "]" << std::endl;
    }
    return 0;
}

