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

void getShoppingList(json request) {
    string path = "lists/" + request["id"].get<string>() + ".json";
    ifstream file(path);

    if (file.is_open()) {
        json list;
        file >> list;
        cout << "Shopping list: " << list.dump(4) << endl;
        file.close();
    } else {
        cout << "Failed to open the file. Ensure the unique ID is correct." << endl;
    }
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

        for (auto& item : request["data"].items()) {
            list["data"][item.key()] = item.value();
        }

        ofstream new_file(path);
        new_file << list.dump(4);
        new_file.close();

        cout << "Shopping list updated successfully" << endl;
    } else {
        cout << "Failed to open the file. Ensure the unique ID is correct." << endl;
    }
}
