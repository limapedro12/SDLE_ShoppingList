#include "handler.hpp"
#include "consistentHashing.hpp"

// MD5 encrypter for hashing
md5 encrypter;

extern ConsistentHashRing hashRing; 

void createShoppingList(json request) {
    int counter = 1;
    // Compute the node using consistent hashing
    string node = hashRing.getNode(request["id"].get<string>())->id;

    // Construct the path for the node and shopping list file
    string path = "nodes/" + node + "/lists/" + request["id"].get<string>() + ".json";

    ifstream file(path);
    while (file.is_open()) {
        file.close();
        counter++;
        path = "nodes/" + node + "/lists/" + request["id"].get<string>() + ".json";
        file.open(path);
    }

    json list = {
        {"id", request["id"]},
        {"data", json::object()}
    };

    ofstream new_file(path);
    new_file << list.dump(4);
    new_file.close();

    cout << "Shopping list created with unique ID: " << request["id"].get<string>() << " on node: " << node << endl;
}

json getShoppingList(json request) {
    // Compute the node using consistent hashing
    string node = hashRing.getNode(request["id"].get<string>())->id;

    // Construct the path for the node and shopping list file
    string path = "nodes/" + node + "/lists/" + request["id"].get<string>() + ".json";

    ifstream file(path);

    if (file.is_open()) {
        json list;
        file >> list;
        file.close();
        return list;
    } else {
        cout << "Failed to open the file on node: " << node << ". Ensure the unique ID is correct." << endl;
    }
    return json();
}

void eraseShoppingList(json request) {
    // Compute the node using consistent hashing
    string node = hashRing.getNode(request["id"].get<string>())->id;

    // Construct the path for the node and shopping list file
    string path = "nodes/" + node + "/lists/" + request["id"].get<string>() + ".json";

    if (remove(path.c_str()) == 0) {
        cout << "Shopping list erased successfully from node: " << node << endl;
    } else {
        cout << "Failed to erase the file from node: " << node << ". Ensure the unique ID is correct." << endl;
    }
}

void mergeShoppingList(json request) {
    // Compute the node using consistent hashing
    string node = hashRing.getNode(request["id"].get<string>())->id;

    // Construct the path for the node and shopping list file
    string path = "nodes/" + node + "/lists/" + request["id"].get<string>() + ".json";
    ifstream file(path);

    if (file.is_open()) {
        json list;
        file >> list;
        file.close();

        // std::cout << "Merging shopping list with unique ID: " << request["id"].get<string>() << " on node: " << node << std::endl;

        // std::cout << list["id"] << std::endl;
        // std::cout << list["data"] << std::endl;
        // std::cout << request["id"] << std::endl;
        // std::cout << request["data"] << std::endl;

        // ShoppingList oldList(list["id"], list["data"]);
        // ShoppingList newList(request["id"], request["data"]);

        // std::cout << "antes merge" << std::endl;

        // oldList.setUserId("Server");

        // ShoppingList listToKeep = oldList.merge(newList);
        // json listToKeepInJson;

        // std::cout << "depois merge" << std::endl;
        
        // listToKeepInJson["id"] = listToKeep.get_id();
        // listToKeepInJson["data"] = listToKeep.contentsToJSON();

        ofstream new_file(path);
        new_file << request.dump(4);
        new_file.close();

        cout << "Shopping list updated successfully on node: " << node << endl;
    } else {
        cout << "Failed to open the file on node: " << node << ". Ensure the unique ID is correct." << endl;
    }
}