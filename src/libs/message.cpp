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