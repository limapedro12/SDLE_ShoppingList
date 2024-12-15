#include "subscriber.hpp"

// Global map to store all received lists
map<string, ShoppingList> all_received_lists;

// Mutex to protect access to all_received_lists
std::mutex all_received_lists_mutex;

void receiveSubscriptions(zmq::socket_t &subscriber, std::string userId, std::atomic<bool>& running) {
    std::string filename = "client/lists/" + userId + "/subscriptions.json";
    std::ifstream file(filename);
    if (file.good()) {
        json j;
        file >> j;
        for (auto& list : j) {
            ShoppingList sl(list["id"], list["data"]);
            all_received_lists[sl.get_id()] = sl;
        }
        file.close();
    }

    while (running.load()) {
        zmq::message_t message;

        // Receive message non-blocking
        if (subscriber.recv(message, zmq::recv_flags::dontwait)) {
            std::string address = std::string(static_cast<char*>(message.data()), message.size());
            std::string contents = s_recv(subscriber);

            json j = json::parse(contents);
            json j_data = j["data"];
            ShoppingList received_list(address, j_data);
            received_list.setUserId(userId);

            // Lock the mutex before accessing the shared map
            std::lock_guard<std::mutex> lock(all_received_lists_mutex);
            if (all_received_lists.find(address) == all_received_lists.end())
                all_received_lists[address] = received_list;
            else
                all_received_lists[address] = all_received_lists[address].merge(received_list);

            // Save to file
            std::ofstream file(filename);
            json j_all_received_lists;
            for (auto& list : all_received_lists) {
                j_all_received_lists.push_back({{"id", list.first}, {"data", list.second.contentsToJSON()}});
            }
            file << j_all_received_lists;
            file.close();
        }
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
