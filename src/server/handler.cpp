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
