#include "handler.hpp"

namespace fs = std::filesystem;

void createShoppingList(json request, const std::string& workerID) {
    std::string dirPath = "nodes/" + workerID + "/lists";
    std::string filePath = dirPath + "/" + request["id"].get<std::string>() + ".json";

    // Ensure the directory exists
    fs::create_directories(dirPath);

    if (fs::exists(filePath)) {
        std::cerr << "Shopping list already exists with ID: " << request["id"].get<std::string>() << std::endl;
        return;
    }

    json list = {
        {"id", request["id"]},
        {"data", json::object()}
    };

    std::ofstream new_file(filePath);
    new_file << list.dump(4);
    new_file.close();

    std::cout << "Shopping list created with unique ID: " << request["id"].get<std::string>() << std::endl;
}


json getShoppingList(json request, const std::string& workerID) {
    std::string filePath = "nodes/" + workerID + "/lists/" + request["id"].get<std::string>() + ".json";

    if (!fs::exists(filePath)) {
        std::cerr << "Shopping list not found for ID: " << request["id"].get<std::string>() << std::endl;
        return json();
    }

    std::ifstream file(filePath);
    json list;
    file >> list;
    file.close();

    return list;
}

json cloneShoppingList(std::string id) {
    std::vector<std::pair<std::string, size_t>> nodeNames;
    std::string nodesDir = "./nodes/";

    // Iterate over the contents of the nodes directory
    for (const auto& entry : fs::directory_iterator(nodesDir)) {
        if (entry.is_directory()) {
            // Extract the folder name (node name)
            // call the hash function on the folder name
            
            std::string nodeName = entry.path().filename().string();
            md5 encrypter;
            std::string hashString = encrypter.encrypt(nodeName);

            // Convert the first 16 characters (8 bytes) of the hash string to a size_t
            size_t hashValue = 0;
            for (size_t i = 0; i < sizeof(size_t) * 2 && i < hashString.size(); i += 2) {
                hashValue = (hashValue << 8) | std::stoi(hashString.substr(i, 2), nullptr, 16);
            }

            // Add the node name and hash value to the vector
            nodeNames.push_back({nodeName, hashValue});

            // print the node name and hash value
            std::cout << "Node: " << nodeName << " Hash: " << hashValue << std::endl;
        }
    }

    // hash the list id
    md5 encrypter;
    std::string hashString = encrypter.encrypt(id);

    std::cout << "List ID: " << id << " Hash: " << hashString << std::endl;

    // look inside the vector for the node that has the smallest hash value greater than the list id hash value
    size_t listHashValue = 0;
    for (size_t i = 0; i < sizeof(size_t) * 2 && i < hashString.size(); i += 2) {
        listHashValue = (listHashValue << 8) | std::stoi(hashString.substr(i, 2), nullptr, 16);
    }

    std::cout << "POS MERDA " << "List Hash Value: " << listHashValue << std::endl;

    std::string cloneNode;
    size_t cloneNodeHash = SIZE_MAX;
    for (const auto& [nodeName, nodeHash] : nodeNames) {
        if (nodeHash > listHashValue && nodeHash < cloneNodeHash) {
            cloneNode = nodeName;
            cloneNodeHash = nodeHash;
        }
    }

    // print the node that is going to be cloned
    std::cout << "Cloning list to node: " << cloneNode << std::endl;

}

void eraseShoppingList(json request, const std::string& workerID) {
    std::string filePath = "nodes/" + workerID + "/lists/" + request["id"].get<std::string>() + ".json";

    if (fs::remove(filePath)) {
        std::cout << "Shopping list erased successfully: " << request["id"].get<std::string>() << std::endl;
    } else {
        std::cerr << "Failed to erase shopping list with ID: " << request["id"].get<std::string>() << std::endl;
    }
}

void mergeShoppingList(json request, const std::string& workerID) {
    std::string filePath = "nodes/" + workerID + "/lists/" + request["id"].get<std::string>() + ".json";

    if (!fs::exists(filePath)) {
        std::cout << "Shopping list not found for merging: " << request["id"].get<std::string>() << std::endl;
        std::cout << "Creating new shopping list with ID: " << request["id"].get<std::string>() << std::endl;
        createShoppingList(request, workerID);
    }

    std::ifstream file(filePath);
    json old_list;
    file >> old_list;
    file.close();

    json new_data = request["data"];
    json& old_data = old_list["data"];

    for (auto& [key, value] : new_data.items()) {
        old_data[key] = value; // Perform merging logic
    }

    std::ofstream new_file(filePath);
    new_file << old_list.dump(4);
    new_file.close();

    std::cout << "Shopping list merged successfully for ID: " << request["id"].get<std::string>() << std::endl;
}
