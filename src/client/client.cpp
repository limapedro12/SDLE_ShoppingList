#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "../server/handler.hpp"

md5 encrypter1;

enum states {
    NO_LIST,
    SELECTING_LIST,
    LIST_SELECTED,
    SHUTDOWN
};

states state = NO_LIST;
std::string user_id;
json j;

void loadUser(){
    std::ifstream file2("server/number.json");
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
}

vector<ShoppingList> loadLists(){
    std::string listFolder = "./client/lists";
    vector<ShoppingList> shopping_lists;
    try{
        for (const auto & entry : std::filesystem::directory_iterator(listFolder)){
            if (entry.is_regular_file() && entry.path().extension() == ".json"){
                std::string listId = entry.path().filename().string();

                std::ifstream file(entry.path());

                if (file.is_open()){
                    nlohmann::json json;
                    file >> json;

                    ShoppingList shopping_list(listId, json["data"]);
                    shopping_lists.push_back(shopping_list);
                }
                else{
                    std::cerr << "Could not open file: " << entry.path() << std::endl;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
    std::cout << "Loaded " << shopping_lists.size() << " shopping lists" << std::endl;

    return shopping_lists;
}

int createList(vector<ShoppingList>& shopping_lists, zmq::socket_t& socket){
    // Update user counter
    int value;
    bool user_found = false;
    // is a for loop really needed? can't we just check if the user_id is in the json object?
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
    // this routine seems dangerous, should this not be handled by the server?
    // i.e. send a request for creating the userid?
    std::ofstream out_file("server/number.json");
    out_file << j.dump(4);
    out_file.close();

    std::string unhashed_list = user_id + "-" + std::to_string(value);
    std::string list_id = encrypter1.encrypt(unhashed_list);

    // create list object and store in memory
    ShoppingList shopping_list(list_id);
    shopping_list.setUserId(user_id);
    shopping_list.fresh();
    shopping_lists.push_back(shopping_list);

    // save to machine
    std::ofstream file("./client/lists/" + list_id + ".json");
    file << shopping_list.contentsToJSON();
    file.close();

    std::cout << "Created list with id: " << list_id << std::endl;

    // send to server
    Message creation("create", list_id, {});
    s_send(socket, creation.toString());

    // receive reply from server
    // todo once we actally send errors when stuff goes wrong on server

    return 0;
}

states mainMenuUI(vector<ShoppingList>& shopping_lists, zmq::socket_t& socket){
    std::cout << std::endl << "1: Create a new list" << std::endl;
    std::cout << "2: Select a list" << std::endl;
    std::cout << "3: Exit" << std::endl;
    int selection;
    std::cin >> selection;
    if (selection == 1){
        createList(shopping_lists, socket);
        return NO_LIST;
    }
    else if (selection == 2){
        return SELECTING_LIST;
    }
    else if (selection == 3){
        return SHUTDOWN;
    }
    else{
        std::cerr << "Invalid selection" << std::endl;
        return NO_LIST;
    }
}

ShoppingList* selectListUI(vector<ShoppingList>& shopping_lists){
    std::cout << std::endl << "Select a list: " << std::endl;
    for (int i = 0; i < shopping_lists.size(); i++){
        std::cout << i << ": " << shopping_lists[i].get_id() << std::endl;
    }
    int selection;
    std::cin >> selection;
    if (selection < 0 || selection >= shopping_lists.size()){
        std::cerr << "Invalid selection" << std::endl;
        return nullptr;
    }
    else{
        state = LIST_SELECTED;
        return &shopping_lists[selection];
    }
}

int alterListUI(ShoppingList* shoppingList){
    std::cout << std::endl << shoppingList->print() << std::endl;
    std::cout << "1: Add an item" << std::endl;
    std::cout << "2: Remove an item" << std::endl;
    std::cout << "3: Change the quantity of an item" << std::endl;
    std::cout << "4: Back" << std::endl;
    int selection;
    std::cin >> selection;
    if (selection == 1){
        std::string item;
        std::cout << "Enter the item to add: ";
        std::cin >> item;
        shoppingList->add(item);
    }
    else if (selection == 2){
        std::string item;
        std::cout << "Enter the item to remove: ";
        std::cin >> item;
        shoppingList->decrease(item);
    }
    else if (selection == 3){
        std::string item;
        int quantity;
        std::cout << "Enter the item to change: ";
        std::cin >> item;
        std::cout << "Enter the new quantity: ";
        std::cin >> quantity;
        shoppingList->set_value(item, quantity);
    }
    else if (selection == 4){
        // saving to machine routine, it's pretty simple but we could wrap it in a function for clarity
        std::ofstream file("./client/lists/" + shoppingList->get_id() + ".json");
        file << shoppingList->contentsToJSON();
        file.close();
        shoppingList = nullptr;
        state = NO_LIST;
    }
    else{
        std::cerr << "Invalid selection" << std::endl;
    }
    return 0;
}


int main() {
    // Initialize ZeroMQ context and socket
    zmq::context_t context(1);                     // 1 thread in the context
    zmq::socket_t socket(context, ZMQ_REQ);     // Create a request socket

    socket.connect("tcp://localhost:5559");

    // Load the user id
    loadUser();

/*
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
*/

    vector<ShoppingList> shopping_lists = loadLists();
    ShoppingList *current_shopping_list = nullptr;

    while (true){
        switch (state){
            case NO_LIST:
                state = mainMenuUI(shopping_lists, socket);
                break;
            case SELECTING_LIST:
                current_shopping_list = selectListUI(shopping_lists);
                break;
            case LIST_SELECTED:
                alterListUI(current_shopping_list);
                break;
            case SHUTDOWN:
                std::cout << "See you next time!" << std::endl;
                return 0;
        }
    }

    return 0;
}

