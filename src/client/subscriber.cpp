#include "subscriber.hpp"

// Global map to store all received lists
map<string, ShoppingList> all_received_lists;

// Mutex to protect access to all_received_lists
std::mutex all_received_lists_mutex;

void receiveSubscriptions(zmq::socket_t &subscriber, string userId){
    while (1) {
        //  Read envelope with address
        std::string address = s_recv(subscriber);
        //  Read message contents
        std::string contents = s_recv(subscriber);
        
        // std::cout << "[" << address << "] " << contents << std::endl;

        json j = json::parse(contents);

        json j_data = j["data"];
        // cout << "1" << endl;
        ShoppingList received_list(address, j_data);
        // cout << "2" << endl;
        received_list.setUserId(userId);
        // cout << "3" << endl;

        // received_list.add("mergado");

        // Lock the mutex before accessing the shared map
        std::lock_guard<std::mutex> lock(all_received_lists_mutex);

        // Add newly received list to shared map
        if (all_received_lists.find(address) == all_received_lists.end())
            all_received_lists[address] = received_list;
        else
            all_received_lists[address] = all_received_lists[address].merge(received_list);
    }
}

ShoppingList getNewVersion(string list_id){
    // Lock the mutex before accessing the shared map
    std::lock_guard<std::mutex> lock(all_received_lists_mutex);

    if (all_received_lists.find(list_id) != all_received_lists.end())
        return all_received_lists[list_id];
    else 
        return ShoppingList(list_id);
}
