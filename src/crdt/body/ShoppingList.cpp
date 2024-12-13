#include "../headers/ShoppingList.hpp"

ShoppingList::ShoppingList(){
  this->id = "";
  this->items = CounterMap();
}

ShoppingList::ShoppingList(string id){
  this->id = id;
  this->items = CounterMap();
}

ShoppingList::ShoppingList(string id, json j){
  this->id = id;
  this->items = CounterMap(j);
}

void ShoppingList::setUserId(string user_id){
  this->user_id = user_id;
}

string ShoppingList::getUserId(){
  return this->user_id;
}

void ShoppingList::resetUserId(){
  this->user_id = "";
}

void ShoppingList::add(string item){
  if(user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  items.add(item, user_id);
}

void ShoppingList::add(string item, int n){
  if (user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  items.add(item, n, user_id);
}

void ShoppingList::decrease(string item){
  if (user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  if(items.get_quantity(item) >= 1)
    items.decrease(item, user_id);
}

void ShoppingList::decrease(string item, int n){
  if (user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  if (!items.contains(item)){
    return;
  }

  int quantity = items.get_quantity(item);
  if (quantity > n)
    items.decrease(item, n, user_id);
  else
    items.decrease(item, quantity, user_id);
}

void ShoppingList::set_value(string item, int value){
  if (user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  items.set_value(item, value, user_id);
}

void ShoppingList::remove(string item){
  if (user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  items.remove(item, user_id);
}

int ShoppingList::get_quantity(string item){
  return items.get_quantity(item);
}

bool ShoppingList::contains(string item){
  return items.contains(item);
}

ShoppingList ShoppingList::copy(){
  ShoppingList new_shopping_list(this->id);
  new_shopping_list.items = this->items;
  new_shopping_list.setUserId(this->user_id);
  this->fresh();
  return new_shopping_list;
}

void ShoppingList::fresh(){
  if (this->user_id == ""){
    throw std::invalid_argument("user_id not set \n Please call setUserId() first");
  }
  this->items.fresh(this->user_id);
}

ShoppingList ShoppingList::merge(ShoppingList other){
  ShoppingList new_shopping_list(this->id);
  new_shopping_list.items = this->items.merge(other.items, this->user_id);

  for (auto &item : new_shopping_list.items.get_items_with_quantity()){
    if (item.second < 0)
      new_shopping_list.items.remove(item.first, this->user_id);
  }

  new_shopping_list.setUserId(this->user_id);

  return new_shopping_list;
}

set<string> ShoppingList::get_items(){
  return this->items.get_items();
}

map<string, int> ShoppingList::get_items_with_quantity(){
  return this->items.get_items_with_quantity();
}

string ShoppingList::get_id(){
  return this->id;
}

string ShoppingList::print(){
  string output = "[id: ";
  output += this->id;
  output += ", items: ";
  output += this->items.print();
  output += "]";
  return output;
}

CounterMap ShoppingList::get_items_with_counter(){
  return this->items;
}

json ShoppingList::contentsToJSON(){
  return this->items.toJSON();
}