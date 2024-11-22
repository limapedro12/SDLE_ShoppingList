#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include "md5.cpp"

using json = nlohmann::json;
using namespace std;

void createShoppingList(string id);
void getShoppingList(string id);
void updateShoppingList(const std::string& id, const std::string& item, int quantity);

md5 encrypter;

int main() {
    std::cout << "Insert user id: ";
    int id;
    std::cin >> id;
    // Encrypt the ID and use the resulting hash as a string
    std::string hashed_id = encrypter.encrypt(std::to_string(id));

    // Read the json file into a json object
    std::ifstream file("request.json");
    json request;
    file >> request;

    if(request["operation"] == "create") {
        cout << "Creating a shopping list" << endl;
        createShoppingList(request["id"]);
    } 

    else if(request["operation"] == "get") {
        cout << "Updating shopping list" << endl;
        updateShoppingList(request["id"], request["item"], request["quantity"]);
    }

    else if(request["operation"] == "erase") {
        cout << "Deleting shopping list" << endl;
    }

    else if(request["operation"] == "merge") {
        cout << "Merging shopping list" << endl;
    }

    return 0;
}

void createShoppingList(string id){
    int counter = 1;

    // Check if a shopping list already exists for the user
    std::string hashed_id = encrypter.encrypt(id);
    string path = "../../lists/" + hashed_id + "-" + to_string(counter) + ".json";
    ifstream file(path);

    // If it does, increment the counter to create a new shopping list
    while(file){
        file.close();
        counter++;
        path = "../../lists/" + hashed_id + "-" + to_string(counter) + ".json";
        file.open(path);
    }

    // Create the new empty shopping list
    ofstream file2(path);
    json list;
    list["id"] = id;
    list["data"] = json::array();
    file2 << list.dump(4);
    file2.close();
}

void getShoppingList(string id){
    cout << "este es el id: " << id << endl;
    string path = "../../lists/" + id + ".json";
    ifstream file(path);
    string line;
    while(getline(file, line)){
        cout << line << endl;
    }
}

void updateShoppingList(const std::string& id, const std::string& item, int quantity) {
    std::string path = "../../lists/" + id + ".json";
    std::ifstream inFile(path);
    std::string line, jsonContent;

    // Read the existing JSON file into a string
    if (inFile.is_open()) {
        while (getline(inFile, line)) {
            jsonContent += line;
        }
        inFile.close();
    } else {
        // If file doesn't exist, start with an empty JSON object
        jsonContent = "{}";
    }

    // Parse the JSON manually
    size_t startPos = jsonContent.find("{");
    size_t endPos = jsonContent.rfind("}");
    if (startPos == std::string::npos || endPos == std::string::npos) {
        jsonContent = "{}"; // Handle invalid JSON
        startPos = 0;
        endPos = 1;
    }

    // Extract the inner content of the JSON object
    std::string jsonBody = jsonContent.substr(startPos + 1, endPos - startPos - 1);

    // Check if the item already exists in the JSON and update it
    size_t itemPos = jsonBody.find("\"" + item + "\"");
    if (itemPos != std::string::npos) {
        size_t valueStart = jsonBody.find(":", itemPos) + 1;
        size_t valueEnd = jsonBody.find(",", valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = jsonBody.length(); // Last item in JSON
        }
        jsonBody.replace(valueStart, valueEnd - valueStart, " " + std::to_string(quantity));
    } else {
        // Add the new item to the JSON
        if (!jsonBody.empty()) {
            jsonBody += ", ";
        }
        jsonBody += "\"" + item + "\": " + std::to_string(quantity);
    }

    // Reconstruct the JSON object
    jsonContent = "{" + jsonBody + "}";

    // Write the updated JSON back to the file
    std::ofstream outFile(path);
    outFile << jsonContent;
    outFile.close();
}
