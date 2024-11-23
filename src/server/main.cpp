#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include "md5.cpp"

using json = nlohmann::json;
using namespace std;

// Function declarations
void createShoppingList(json request);
void getShoppingList(json request);
void eraseShoppingList(json request);
void updateShoppingList(const std::string& id, const std::string& item, int quantity);

// MD5 encrypter for hashing
md5 encrypter;

int main() {
    cout << "Choose an option: " << endl
         << "1. Create a shopping list" << endl
         << "2. Get a shopping list" << endl
         << "3. Erase a shopping list" << endl
         << "4. Update shopping list" << endl;
    int option;
    cin >> option;
    cout << endl;

    json request;
    ifstream file;

    switch (option)
    {
    case 1:
        file.open("createRequest.json");
        file >> request;
        file.close();
        createShoppingList(request);
        break;
    case 2: {
        file.open("getRequest.json");
        file >> request;
        file.close();
        getShoppingList(request);
        break;
    }
    case 3: {
        file.open("eraseRequest.json");
        file >> request;
        file.close();
        eraseShoppingList(request);
        break;
    }
    default:
        cout << "Invalid option" << endl;
        break;
    }

    return 0;
}

void createShoppingList(json request) {
    int counter = 1;
    //try to open the list with the id = hashed_id from the json + hashed counter
    string hashed_id = encrypter.encrypt(request["id"].get<string>() + to_string(counter));
    string path = "../lists/" + hashed_id + ".json";
    ifstream file(path);

    while (file.is_open()) {
        file.close();
        counter++;
        hashed_id = encrypter.encrypt(request["id"].get<string>() + to_string(counter));
        path = "../lists/" + hashed_id + ".json";
        file.open(path);
    }

    json list = {
        {"id", request["id"]},
        {"data", json::object()}
    };

    ofstream new_file(path);
    new_file << list.dump(4);
    new_file.close();

    cout << "Shopping list created with unique ID: " << hashed_id << endl;
}

void getShoppingList(json request) {
    string path = "../lists/" + request["id"].get<string>() + ".json";
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
    string path = "../lists/" + request["id"].get<string>() + ".json";
    if (remove(path.c_str()) == 0) {
        cout << "Shopping list erased successfully" << endl;
    } else {
        cout << "Failed to erase the file. Ensure the unique ID is correct." << endl;
    }
}
