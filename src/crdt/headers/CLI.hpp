#include <string>
#include <iostream>
#include <vector>
#include "ShoppingList.hpp"

using namespace std;

void add(ShoppingList& shopping_list);

void decrease(ShoppingList& shopping_list);

void set_value(ShoppingList &shopping_list);

void reset(ShoppingList &shopping_list);

string generate_id();

void remove(vector<ShoppingList> &shopping_lists);

ShoppingList *get(vector<ShoppingList> &shopping_lists);

void list(vector<ShoppingList> &shopping_lists);

void create(vector<ShoppingList> &shopping_lists);

void push(ShoppingList &shopping_list);

void pull(ShoppingList &shopping_list);
