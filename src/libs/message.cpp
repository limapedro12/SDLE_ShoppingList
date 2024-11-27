#include "message.h"

Message::Message(nlohmann::json json){
    this->json = json;
    this->operation = json["operation"];
    this->id = json["id"];
    this->data = json["data"];
}

Message::Message(std::string json){
    this->json = nlohmann::json::parse(json);
    this->operation = this->json["operation"];
    this->id = this->json["id"];
    this->data = this->json["data"];
}

Message::Message(ShoppingList shoppingList, std::string operation){
    this->operation = operation;
    this->id = shoppingList.get_id();
    this->data = CRDTCounterMapToJSON(shoppingList.get_items_with_counter());
    this->json["operation"] = operation;
    this->json["id"] = this->id;
    this->json["data"] = this->data;
}

nlohmann::json Message::CRDTCounterMapToJSON(CRDTCounterMap items){
    nlohmann::json json;
    for (auto item : items){
        json[item.first] = CRDTCounterToJSON(item.second);
    }
    return json;
}

nlohmann::json Message::CRDTCounterToJSON(CRDTCounter counter){
    nlohmann::json json;
    json["causalHistory"] = counter.get_causal_history().getMap();
    json["counter"] = counter.get_map();
    return json;
}

Message::Message(std::string operation, std::string id, std::unordered_map <std::string, int> data){
    this->operation = operation;
    this->id = id;
    this->data = data;
    this->json["operation"] = operation;
    this->json["id"] = id;
    this->json["data"] = data;
}

Message::~Message(){
}

std::string Message::getOperation(){
    return this->operation;
}

std::string Message::getId(){
    return this->id;
}

std::unordered_map <std::string, int> Message::getData(){
    return this->data;
}

nlohmann::json Message::getJson(){
    return this->json;
}

void Message::setOperation(std::string operation){
    this->operation = operation;
    this->json["operation"] = operation;
}

void Message::setId(std::string id){
    this->id = id;
    this->json["id"] = id;
}

void Message::setData(std::unordered_map <std::string, int> data){
    this->data = data;
    this->json["data"] = data;
}

void Message::setJson(nlohmann::json json){
    this->json = json;
    this->operation = json["operation"];
    this->id = json["id"];
    this->data = json["data"];
}

std::string Message::toString(){
    return this->json.dump();
}