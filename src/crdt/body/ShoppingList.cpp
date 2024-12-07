#include "../headers/ShoppingList.hpp"


ShoppingList::ShoppingList(string id){
  this->id = id;
  this->items = CounterMap();
}

void ShoppingList::setUserId(string user_id){
  this->user_id = user_id;
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
    items.decrease(item, user_id);
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

ShoppingList::ShoppingList(string id, json j){
  this->id = id;
  this->items = CounterMap(j);
}

// int main(){
//   string user_id = "1";
//   string user_id2 = "2";
//   ShoppingList shopping_list("1");
//   shopping_list.setUserId(user_id);

//   cout << "Original shopping list: " << endl;
//   shopping_list.add("apple");
//   shopping_list.add("banana");
//   shopping_list.add("apple", 3);
//   shopping_list.add("pinapple");

//   cout << shopping_list.print() << endl
//        << endl;

//   ShoppingList shopping_list_copy = shopping_list.copy();
//   shopping_list_copy.setUserId(user_id2);
//   shopping_list_copy.add("orange");
//   shopping_list_copy.add("apple", 2);
//   shopping_list_copy.add("banana", 20);
//   shopping_list_copy.decrease("banana", 21);
//   shopping_list_copy.remove("pinapple");

//   shopping_list.decrease("apple");
//   shopping_list.decrease("banana", 2);
//   shopping_list.add("pinapple");

//   cout << "Shopping list after user 1:" << endl;
//   cout << shopping_list.print() << endl
//        << endl;

//   cout << "Shopping list after user 2:" << endl;
//   cout << shopping_list_copy.print() << endl
//        << endl;

//   // cout << shopping_list_copy.contentsToJSON().dump(4) << endl;

//   cout << "Merged shopping list:" << endl;
//   cout << shopping_list.merge(shopping_list_copy).print() << endl
//        << endl;
       
//   // ShoppingList jsonShopping("123", shopping_list_copy.contentsToJSON());

//   // cout << "JSON" << endl;
//   // cout << jsonShopping.contentsToJSON().dump(4) << endl;
// }
