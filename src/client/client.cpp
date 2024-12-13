#include "../libs/zhelpers.hpp"
#include "../libs/message.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <thread>
#include <map>
#include "../server/handler.hpp"
#include "subscriber.hpp"

md5 encrypter1;

enum states {
    NO_LIST,
    SELECTING_LIST,
    CLONE_LIST,
    LIST_SELECTED,
    SHUTDOWN,
    SETTINGS
};

states state = NO_LIST;
std::string user_id = "";
json j;
std::unordered_map<std::string, bool> settings;

void loadUser(){
    std::ifstream file2("server/number.json");
    file2 >> j;
    file2.close();

    // check if there is a info.json file in the client dir, if there is not, create one and put hashed id in it, else read the hashed id from the file
    if (!std::filesystem::exists("client/info.json")){
        std::ofstream file_out("client/info.json");
        
        // Write the user_id to the info.json file with the key user_id
        json user_id_json = {{"user_id", std::unordered_map<std::string, std::string>{}}};

        // add basic settings
        user_id_json["settings"]["automatic_push"] = true;
        user_id_json["settings"]["automatic_pull"] = true;

        file_out << user_id_json.dump(4);
        file_out.close();
    }

    std::ifstream file("client/info.json");

    json client_json;
    file >> client_json;
    file.close();
    std::unordered_map<std::string, std::string> user_ids = client_json["user_id"];
    settings["automatic_push"] = client_json["settings"]["automatic_push"];
    settings["automatic_pull"] = client_json["settings"]["automatic_pull"];

    while (true){
        std::cout << std::endl << "Do you want to login or create a new user?" << std::endl;
        std::cout << "1: Login" << std::endl;
        std::cout << "2: Create new user" << std::endl;	
        int selection;
        std::cin >> selection;

        if (selection != 1 && selection != 2){
            std::cerr << "Invalid selection" << std::endl;
            continue;
        }
        while (true){
            if (selection==1){
                std::cout << std::endl << "Choose a user to login: " << std::endl;
                std::cout << "0: Go back" << std::endl;
                std::vector<std::string> number_to_id;
                int i = 1;
                for (auto &user : user_ids){
                    number_to_id.push_back(user.first);
                    std::cout << i << ": ";
                    user.second == "" ? std::cout << user.first : std::cout << user.second << " - " << user.first;
                    std::cout << std::endl;
                    i++;
                }
                int user_selection;
                std::cin >> user_selection;
                if (user_selection < 0 || user_selection > user_ids.size()){
                    std::cerr << "Invalid selection" << std::endl;
                    continue;
                }
                else if (user_selection == 0){
                    break;
                }
                else{
                    std::string index = number_to_id[user_selection-1];
                    user_id = index;
                    break;
                }
            }
            else{
                // Check the number.json file in the server directory and get the value of the number key, hash it to use as the hashed id and increment the number value by 1
                int number = j["number"];
                j["number"] = number + 1;
                std::string new_user_id = encrypter1.encrypt(std::to_string(number));
                user_id = new_user_id;

                // Add user_id to the info.json file
                // make sure we have the latest version of the file
                std::ifstream file_in("client/info.json");
                file_in >> client_json;
                file_in.close();
                client_json["user_id"][new_user_id] = "";
                std::ofstream file_out("client/info.json");
                file_out << client_json.dump(4);
                file_out.close();

                // Add the new user to user_numbers
                json new_user = {{user_id, 1}};
                j["user_numbers"].push_back(new_user);

                // Save the updated number.json file
                std::ofstream out_file("server/number.json");
                out_file << j.dump(4);
                out_file.close();

                break;
            }
        }

        if (user_id != ""){
            if (client_json["user_id"]=="") std::cout << "Logged in as user: " << user_id << std::endl;
            else std::cout << "Logged in as user: " << client_json["user_id"][user_id] << std::endl;
            break;
        }
    }

}

void mergeAllListsFromServer(vector<ShoppingList> &shopping_lists, zmq::socket_t &socket){
    for (auto &shopping_list : shopping_lists){
        string list_id = shopping_list.get_id();
        // send to server
        Message cloneMessage("get", list_id, {});
        s_send(socket, cloneMessage.toString());

        // receive the list from the server, parse to json and save to file in the client dir
        std::string list_json = s_recv(socket);
        json list = json::parse(list_json);

        cout << "Received list: " << list_id << std::endl;

        ShoppingList newList(list_id, list);

        shopping_list = shopping_list.merge(newList);
    }
}

vector<ShoppingList> loadLists(){
    std::string listFolder = "./client/lists/"+user_id;
    vector<ShoppingList> shopping_lists;
    if (!std::filesystem::exists(listFolder)){
        std::filesystem::create_directory(listFolder);
    }
    else{
        try{
            for (const auto & entry : std::filesystem::directory_iterator(listFolder)){
                if (entry.is_regular_file() && entry.path().extension() == ".json"){
                    std::string listId = entry.path().filename().string();
                    listId.erase(listId.length() - 5, 5);

                    std::ifstream file(entry.path());

                    if (file.is_open()){
                        nlohmann::json json;
                        file >> json;

                        ShoppingList shopping_list(listId, json);
                        shopping_list.setUserId(user_id);
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
    }

    std::cout << "Loaded " << shopping_lists.size() << " shopping lists" << std::endl;

    return shopping_lists;
}

int createList(vector<ShoppingList> &shopping_lists, zmq::socket_t &socket, zmq::socket_t &subscriber){
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
    std::ofstream file("./client/lists/"+ user_id + "/" + list_id + ".json");
    file << shopping_list.contentsToJSON();
    file.close();

    std::cout << "Created list with id: " << list_id << std::endl;

    // send to server
    Message creation("create", list_id, {});
    s_send(socket, creation.toString());

    // receive reply from server
    std::cout << "Received reply from server " << s_recv(socket) << std::endl;
    // todo once we actally send errors when stuff goes wrong on server
 
    // subscribe to list
    subscriber.set(zmq::sockopt::subscribe, list_id);

    return 0;
}

void cloneList(std::string list_id, zmq::socket_t& socket, vector<ShoppingList>& shopping_lists, zmq::socket_t& subscriber){
    
    // send to server
    Message cloneMessage("get", list_id, {});
    s_send(socket, cloneMessage.toString());

    // receive the list from the server, parse to json and save to file in the client dir
    std::string list_json = s_recv(socket);
    std::cout << "Received list: " << list_json << std::endl;
    json list = json::parse(list_json);

    //add to shopping lists, if already exists, merge
    bool found = false;
    for (auto& shopping_list : shopping_lists){
        if (shopping_list.get_id() == list_id){
            found = true;
            shopping_list = shopping_list.merge(ShoppingList(list_id, list));
            break;
        }
    }

    if (!found){
        ShoppingList shopping_list(list_id, list["data"]);
        shopping_list.setUserId(user_id);
        shopping_lists.push_back(shopping_list);
        subscriber.set(zmq::sockopt::subscribe, list_id);
    }

    cout << "Added to shopping lists" << endl;

    std::ofstream file("./client/lists/" + list_id + ".json");
    file << list;
    file.close();
}

states mainMenuUI(vector<ShoppingList> &shopping_lists, zmq::socket_t &socket, zmq::socket_t &subscriber){
    std::cout << std::endl << "1: Create a new list" << std::endl;
    std::cout << "2: Select a list" << std::endl;
    std::cout << "3: Change user" << std::endl;
    std::cout << "4: Clone a list" << std::endl;
    std::cout << "5: Settings" << std::endl;
    std::cout << "6: Exit" << std::endl;  

    int selection;
    std::cin >> selection;
    if (selection == 1){
        createList(shopping_lists, socket, subscriber);
        return NO_LIST;
    }
    else if (selection == 2){
        return SELECTING_LIST;
    }
    else if (selection == 3){
        loadUser();
        shopping_lists = loadLists();
        return NO_LIST;
    }
    else if (selection == 4){
        std::cout << "Please enter the ID of the list you want to clone: " << std::endl;
        std::string clone_id;
        std::cin >> clone_id;
        cloneList(clone_id, socket, shopping_lists, subscriber);
        return CLONE_LIST;
    }
    else if (selection == 5){
        return SETTINGS;
    }
    else if (selection == 6){
        return SHUTDOWN;
    }
    else{
        std::cerr << "Invalid selection" << std::endl;
        return NO_LIST;
    }
}

ShoppingList* selectListUI(vector<ShoppingList>& shopping_lists){
    std::cout << std::endl << "Select a list: " << std::endl;
    std::cout << "0: Go back" << std::endl;
    for (int i = 0; i < shopping_lists.size(); i++){
        std::cout << i+1 << ": " << shopping_lists[i].get_id() << std::endl;
    }
    int selection;
    std::cin >> selection;
    if (selection < 0 || selection > shopping_lists.size()){
        std::cerr << "Invalid selection" << std::endl;
        return nullptr;
    }
    else if (selection == 0) {
        state = NO_LIST;
        return nullptr;
    }
    else{
        state = LIST_SELECTED;
        return &shopping_lists[selection-1];
    }
}



int alterListUI(ShoppingList* shoppingList, ShoppingList originalList, zmq::socket_t& socket){
    if(settings["automatic_pull"]){
        *(shoppingList) = shoppingList->merge(getNewVersion(shoppingList->get_id()));
    }

    std::cout << std::endl << shoppingList->print() << std::endl;
    std::cout << "1: Add/increase an item" << std::endl;
    std::cout << "2: Decrease an item" << std::endl;
    std::cout << "3: Remove an item" << std::endl;
    std::cout << "4: Change the quantity of an item" << std::endl;
    settings["automatic_push"] ? std::cout << "5: Save, push list, and go back" << std::endl : std::cout << "5: Save and go back" << std::endl;
    std::cout << "6: Go back without saving" << std::endl;
    int selection;
    std::cin >> selection;
    if (selection == 1){
        std::string item;
        std::cout << "Enter the item to add or increase: ";
        std::cin >> item;
        std::cout << "Enter the quantity: ";
        int quantity;
        std::cin >> quantity;
        shoppingList->add(item, quantity);
    }
    else if (selection == 2){
        std::string item;
        std::cout << "Enter the item to decrease: ";
        std::cin >> item;
        std::cout << "Enter the quantity: ";
        int quantity;
        std::cin >> quantity;
        shoppingList->decrease(item, quantity);
    }
    else if (selection == 3){
        std::string item;
        std::cout << "Enter the item to remove: ";
        std::cin >> item;
        shoppingList->decrease(item, shoppingList->get_quantity(item));
    }
    else if (selection == 4){
        std::string item;
        int quantity;
        std::cout << "Enter the item to change: ";
        std::cin >> item;
        std::cout << "Enter the new quantity: ";
        std::cin >> quantity;
        shoppingList->set_value(item, quantity);
    }
    else if (selection == 5){
        shoppingList->fresh();

        // saving to machine routine, it's pretty simple but we could wrap it in a function for clarity
        std::ofstream file("./client/lists/" + user_id + "/" + shoppingList->get_id() + ".json");
        file << shoppingList->contentsToJSON();
        file.close();

        //send to server
        if (settings["automatic_push"]){
            Message mergeMessage(*shoppingList, "merge");
            s_send(socket, mergeMessage.toString());
            s_recv(socket);
        }

        shoppingList = nullptr;
        state = NO_LIST;
    }
    else if (selection == 6){
        // std::cout << shoppingList->print() << std::endl;
        // std::cout << originalList.print() << std::endl
        //           << std::endl;
        *(shoppingList) = originalList.copy();
        // shoppingList = nullptr;
        state = NO_LIST;
    //     std::cout << shoppingList->print() << std::endl;
    //     std::cout << originalList.print() << std::endl
    //               << std::endl;
    }
    else{
        std::cerr << "Invalid selection" << std::endl;
    }
    return 0;
}



int innerSettingsUI(json& settings_json){
    std::cout << std::endl << "1: Toggle automatic push: " << settings_json["settings"]["automatic_push"] << std::endl;
    std::cout << "2: Toggle automatic pull: " << settings_json["settings"]["automatic_pull"] << std::endl;
    std::cout << "3: Change nickname" << std::endl;
    std::cout << "4: Back" << std::endl;

    int selection;
    std::cin >> selection;
    if (selection == 1){
        settings_json["settings"]["automatic_push"] = !settings_json["settings"]["automatic_push"];
        settings["automatic_push"] = !settings["automatic_push"];
    }
    else if (selection == 2){
        settings_json["settings"]["automatic_pull"] = !settings_json["settings"]["automatic_pull"];
        settings["automatic_pull"] = !settings["automatic_pull"];
    }
    else if (selection == 3){
        std::string nickname;
        std::cout << "Enter new nickname: ";
        std::cin >> nickname;
        settings_json["user_id"][user_id] = nickname;
    }
    else if (selection == 4){
        return 1;
    }
    else{
        std::cerr << "Invalid selection" << std::endl;
    }

    return 0;
}

int settingsUI(){
    std::cout << std::endl << "Settings" << std::endl;
    std::ifstream settings_file("client/info.json");
    json settings_json;
    settings_file >> settings_json;
    settings_file.close();

    while (innerSettingsUI(settings_json) == 0);

    std::ofstream file("client/info.json");
    file << settings_json.dump(4);
    file.close();


    return 0;
}




int main() {
    // Initialize ZeroMQ context and socket
    zmq::context_t context(1);                     // 1 thread in the context
    zmq::socket_t socket(context, ZMQ_REQ);     // Create a request socket

    // Load the user id
    loadUser();

    srand(time(NULL));
    std::string zmqID = s_set_id(socket);
    socket.connect("tcp://localhost:5559");

    // Initialize subscriber socket
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5561");

    vector<ShoppingList> shopping_lists = loadLists();

    // mergeAllListsFromServer(shopping_lists, socket);

    for(auto &list : shopping_lists){
        subscriber.set(zmq::sockopt::subscribe, list.get_id());
    }

    thread subscriptionThread(receiveSubscriptions, std::ref(subscriber), user_id);

    ShoppingList *current_shopping_list = nullptr;

    ShoppingList originalList;

    while (true){
        switch (state){
            case NO_LIST:
                state = mainMenuUI(shopping_lists, socket, subscriber);
                break;
            case SELECTING_LIST:
                current_shopping_list = selectListUI(shopping_lists);
                originalList = current_shopping_list->copy();
                break;
            case CLONE_LIST:
                state = NO_LIST;
                break;
            case LIST_SELECTED:
                alterListUI(current_shopping_list, originalList, socket);
                break;
            case SETTINGS:
                settingsUI();
                state = NO_LIST;
                break;
            case SHUTDOWN:
                std::cout << "See you next time!" << std::endl;
                return 0;
        }
    }

    return 0;
}
