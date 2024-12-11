#include "subscriber.hpp"

int receiveSubscriptions(zmq::socket_t &subscriber, vector<ShoppingList>& shopping_lists){
    while (1) {
      //  Read envelope with address
      std::string address = s_recv (subscriber);
      //  Read message contents
      std::string contents = s_recv (subscriber);
		
      std::cout << "[" << address << "] " << contents << std::endl;
    }
    return 0;
}

// #include "subscriber.hpp"

// int receiveSubscriptions(zmq::socket_t &subscriber, vector<ShoppingList>& shopping_lists){
//     // Subscribe to all lists in shopping_lists
//     unsigned last_index = shopping_lists.size();

//     for (auto &list : shopping_lists){
//         subscriber.set(zmq::sockopt::subscribe, list.get_id());
//         cout << "Subscribed to " << list.get_id() << endl;
//     }

//     zmq::pollitem_t poller = {subscriber, 0, ZMQ_POLLIN, 0};
//     while (1) {
//         cout << shopping_lists.size() << endl;
//         // Process new lists
//         if (last_index > shopping_lists.size()){
//             for (unsigned i = last_index; i < shopping_lists.size(); i++){
//                 subscriber.set(zmq::sockopt::subscribe, shopping_lists[i].get_id());
//                 cout << "Subscribed to " << shopping_lists[i].get_id() << endl;
//             }
//             last_index = shopping_lists.size();
//         }

//         zmq::poll(&poller, 1, -1);
//         // See if queue has any messages
//         if (poller.revents & ZMQ_POLLIN){
//             //  Read envelope with address
//             cout << "Received message" << endl;
//             string address = s_recv (subscriber);
//             //  Read message contents
//             string contents = s_recv (subscriber);
                
//             cout << "[" << address << "] " << contents << endl;
//         }

//         cout << "Waiting for message" << endl;
//     }
//     return 0;
// }

