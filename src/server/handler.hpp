#include <iostream>
#include <fstream>
#include <string>
#include "../libs/json.hpp"
#include "../crdt/headers/ShoppingList.hpp"
#include "md5.cpp"
#include "consistentHashing.hpp"
#include <queue>

using json = nlohmann::json;
using namespace std;

// Function declarations
void createShoppingList(json request, const std::string& workerID);
json getShoppingList(json request, const std::string& workerID);
void eraseShoppingList(json request, const std::string& workerID);
json cloneShoppingList(std::string id);
json mergeShoppingList(json request, const std::string& workerID);
