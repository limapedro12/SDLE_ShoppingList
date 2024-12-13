#include "subscriber.hpp"

// Global map to store all received lists
map<string, ShoppingList> all_received_lists;

// Mutex to protect access to all_received_lists
std::mutex all_received_lists_mutex;

void receiveSubscriptions(zmq::socket_t &subscriber, string userId){
    // read from file lists/userId/subscriptions.json and save it to all_received_lists
    string filename = "client/lists/" + userId + "/subscriptions.json";
    ifstream file(filename);
    if (file.good()){
        json j;
        file >> j;
        for (auto &list : j){
            ShoppingList sl(list["id"], list["data"]);
            all_received_lists[sl.get_id()] = sl;
        }
        file.close();
    }

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

        // save to file
        ofstream file(filename);
        // create a json list and for each member in all_received lists to it
        json j_all_received_lists;
        for (auto &list : all_received_lists){
            j_all_received_lists.push_back({{"id", list.first},
                                            {"data", list.second.contentsToJSON()}});
        }
        file << j_all_received_lists;
        file.close();
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
