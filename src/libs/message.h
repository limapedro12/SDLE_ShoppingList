#include <string>
#include <vector>
#include <unordered_map>
#include "json.hpp"
#include "../crdt/headers/ShoppingList.hpp"

enum Operation {
    HELLO_WORLD,
    CREATE,
    GET,
    MERGE,
    ERASE
};

class Message{
    public:
        Message(nlohmann::json json);
        Message(std::string json);
        Message(std::string operation, std::string id, std::unordered_map <std::string, int> data);
        Message(ShoppingList shoppingList, std::string operation);
        nlohmann::json CRDTCounterMapToJSON(CRDTCounterMap items);
        nlohmann::json CRDTCounterToJSON(CRDTCounter counter);
        ~Message();
        std::string getOperation();
        std::string getId();
        std::unordered_map <std::string, int> getData();
        nlohmann::json getJson();
        void setOperation(std::string operation);
        void setId(std::string id);
        void setData(std::unordered_map <std::string, int> data);
        void setJson(nlohmann::json json);
        std::string toString();

    private:
        std::string operation;
        std::string id;
        nlohmann::json data;
        nlohmann::json json;
};