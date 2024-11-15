#include <iostream>
#include <fstream>
#include <set>

using namespace std;

class Server{
    public:

    // gets the user's shopping list from the server, the list is in a hashed txt file in the lists folder and is fetched by the user's id, and printed to the console
    void getShoppingList(string id){
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

};