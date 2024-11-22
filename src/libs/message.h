#include <string>
#include <vector>
#include <unordered_map>
#include "json.hpp"

class Message{
    public:
        Message(nlohmann::json json);
        Message(std::string json);
        Message(std::string operation, std::string id, std::unordered_map <std::string, int> data);
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
        std::unordered_map <std::string, int> data;
        nlohmann::json json;
};