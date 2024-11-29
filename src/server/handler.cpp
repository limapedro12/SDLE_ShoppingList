#include "handler.hpp"

// MD5 encrypter for hashing
md5 encrypter;

int main1() {
    cout << "Choose an option: " << endl
         << "1. Create a shopping list" << endl
         << "2. Get a shopping list" << endl
         << "3. Erase a shopping list" << endl
         << "4. Merge shopping list" << endl;
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
    case 4: {
        file.open("mergeRequest.json");
        file >> request;
        file.close();
        mergeShoppingList(request);
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
