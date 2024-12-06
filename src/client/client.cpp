#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <iostream>
#include <string>
#include <vector>
#include "../server/handler.hpp"

md5 encrypter1;

int main() {
    // Initialize ZeroMQ context and socket
    zmq::context_t context(1);                     // 1 thread in the context
    zmq::socket_t socket(context, ZMQ_REQ);     // Create a request socket

    socket.connect("tcp://localhost:5559");

    std::string user_id;
    std::ifstream file2("server/number.json");
    json j;
    file2 >> j;
    file2.close();

    // check if there is a info.json file in the client dir, if there is not, create one and put hashed id in it, else read the hashed id from the file
    std::ifstream file("client/info.json");
    if (!file.good()) {
        std::ofstream file("client/info.json");
        // Check the number.json file in the server directory and get the value of the number key, hash it to use as the hashed id and increment the number value by 1
        int number = j["number"];
        j["number"] = number + 1;
        user_id = encrypter1.encrypt(std::to_string(number));
        // Write the user_id to the info.json file with the key user_id
        json user_id_json = {{"user_id", user_id}};
        file << user_id_json;
        file.close();
        // Add the new user to user_numbers
        json new_user = {{user_id, 1}};
        j["user_numbers"].push_back(new_user);

    } else {
        json client_json;
        file >> client_json;
        user_id = client_json["user_id"];
        file.close();
    }

    // Update user counter
    int value;
    bool user_found = false;
    for (auto& user : j["user_numbers"]) { 
        if (user.contains(user_id)) {     
            user[user_id] = user[user_id].get<int>() + 1; 
            value = user[user_id].get<int>();
            user_found = true;
            break;
        }
    }

    if (!user_found) {
        json new_user = {{user_id, 1}};
        value = 1;
        j["user_numbers"].push_back(new_user);
    }

    // Save the updated number.json file
    std::ofstream out_file("server/number.json");
    out_file << j.dump(4);
    out_file.close();

    std::string unhashed_list = user_id + "-" + std::to_string(value);
    std::string list_id = encrypter1.encrypt(unhashed_list);

    Message creation("create", list_id, {});
    s_send(socket, creation.toString());

    std::string reply_str = s_recv(socket);

    std::cout << "Received reply :" << " [" << reply_str << "]" << std::endl;

    for (int request_nbr = 0; request_nbr < 10; ++request_nbr) {
        // Create the message object
        //std::unordered_map<std::string, int> data = {{"a", 1}, {"b", 2}, {"c", 3}};
        //Message message("helloWorld", "mrBombastic2", data);
        ShoppingList shopping_list(list_id);
        shopping_list.setUserId(user_id);
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

