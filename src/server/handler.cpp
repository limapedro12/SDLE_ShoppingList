#include "handler.hpp"

// MD5 encrypter for hashing
md5 encrypter;

void createShoppingList(json request) {
    int counter = 1;
    //try to open the list with the id = hashed_id from the json + hashed counter
    string path = "lists/" + request["id"].get<string>() + ".json";
    ifstream file(path);

    while (file.is_open()) {
        file.close();
        counter++;
        path = "lists/" + request["id"].get<string>() + ".json";
        file.open(path);
    }

    json list = {
        {"id", request["id"]},
        {"data", json::object()}
    };

    ofstream new_file(path);
    new_file << list.dump(4);
    new_file.close();

    cout << "Shopping list created with unique ID: " << request["id"].get<string>() << endl;
}

json getShoppingList(json request) {
    string path = "lists/" + request["id"].get<string>() + ".json";
    ifstream file(path);

    if (file.is_open()) {
        json list;
        file >> list;
        file.close();
        return list;
    } else {
        cout << "Failed to open the file. Ensure the unique ID is correct." << endl;
    }
    return json();
}

void eraseShoppingList(json request) {
    string path = "lists/" + request["id"].get<string>() + ".json";
    if (remove(path.c_str()) == 0) {
        cout << "Shopping list erased successfully" << endl;
    } else {
        cout << "Failed to erase the file. Ensure the unique ID is correct." << endl;
    }
}

void mergeShoppingList(json request) {
    string path = "lists/" + request["id"].get<string>() + ".json";
    ifstream file(path);

    if (file.is_open()) {
        json list;
        file >> list;
        file.close();

        ShoppingList oldList(list["id"], list["data"]);
        ShoppingList newList(request["id"], request["data"]);

        ShoppingList listToKeep = oldList.merge(newList);
        json listToKeepInJson;
        
        listToKeepInJson["id"] = listToKeep.get_id();
        listToKeepInJson["data"] = listToKeep.contentsToJSON();

        ofstream new_file(path);
        new_file << listToKeepInJson.dump(4);
        new_file.close();

        cout << "Shopping list updated successfully" << endl;
    } else {
        cout << "Failed to open the file. Ensure the unique ID is correct." << endl;
    }
}
