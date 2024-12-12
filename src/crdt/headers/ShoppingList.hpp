#include "string"
#include "set"
#include "map"
#include "../../libs/json.hpp"
#include "CounterMap.hpp"

using namespace std;
using json = nlohmann::json;

#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

class ShoppingList{
private:
  CounterMap items;
  string id;
  string user_id = "";

public:
  ShoppingList();

  ShoppingList(string id);

  ShoppingList(string id, json j);

  void setUserId(string user_id);

  string getUserId();

  void resetUserId();

  void add(string item);

  void add(string item, int n);

  void decrease(string item);

  void decrease(string item, int n);

  void set_value(string item, int value);

  void remove(string item);

  int get_quantity(string item);

  bool contains(string item);

  ShoppingList copy();

  void fresh();

  ShoppingList merge(ShoppingList other);

  set<string> get_items();

  map<string, int> get_items_with_quantity();

  CounterMap get_items_with_counter();
  
  string get_id();

  string print();

  json contentsToJSON();

};

#endif
