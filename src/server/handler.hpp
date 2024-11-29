#include <iostream>
#include <fstream>
#include <string>
#include "../libs/json.hpp"
#include "../crdt/headers/ShoppingList.hpp"
#include "md5.cpp"

using json = nlohmann::json;
using namespace std;

// Function declarations
void createShoppingList(json request);
void getShoppingList(json request);
void eraseShoppingList(json request);
void mergeShoppingList(json request);