#include "../headers/ShoppingList.hpp"


ShoppingList::ShoppingList(string id){
  this->id = id;
  this->items = CRDTCounterMap();
}

void ShoppingList::set_user_id(int user_id){
  this->user_id = user_id;
}

void ShoppingList::reset_user_id(){
  this->user_id = 0;
}

void ShoppingList::add(string item){
  if(user_id == 0){
    throw std::invalid_argument("user_id not set \n Please call set_user_id() first");
  }
  if (items.find(item) == items.end()){
    items[item] = CRDTCounter();
  }
  items[item].incr(user_id);
}

void ShoppingList::add(string item, int n){
  if (user_id == 0){
    throw std::invalid_argument("user_id not set \n Please call set_user_id() first");
  }
  if (items.find(item) == items.end()){
    items[item] = CRDTCounter();
  }
  items[item].incr(n, user_id);
}

void ShoppingList::decrease(string item){
  if (user_id == 0){
    throw std::invalid_argument("user_id not set \n Please call set_user_id() first");
  }
  if (items.find(item) == items.end()){
    return;
  }
  if (items[item].value() < 1){
    return;
  }
  items[item].decr(user_id);
}

void ShoppingList::decrease(string item, int n){
  if (user_id == 0){
    throw std::invalid_argument("user_id not set \n Please call set_user_id() first");
  }
  if (items.find(item) == items.end()){
    return;
  }

  if (items[item].value() > n)
    items[item].decr(n, user_id);
  else
    items[item].reset();
}

void ShoppingList::set_value(string item, int value){
  if (items.find(item) == items.end()){
    return;
  }
  items[item].set_value(value, user_id);
}

void ShoppingList::reset(string item){
  if (items.find(item) == items.end()){
    return;
  }
  items[item].reset();
}

int ShoppingList::get_quantity(string item){
  if (items.find(item) == items.end()){
    return 0;
  }
  return items[item].value();
}

bool ShoppingList::is_item_present(string item){
  if (items.find(item) == items.end()){
    return false;
  }
  return items[item].value() > 0;
}

ShoppingList ShoppingList::copy(){
  ShoppingList new_shopping_list(this->id);
  new_shopping_list.items = this->items;
  this->fresh();
  return new_shopping_list;
}

void ShoppingList::fresh(){
  for (auto &item : items){
    item.second.fresh(user_id);
  }
}

ShoppingList ShoppingList::merge(ShoppingList other){
  ShoppingList new_shopping_list(this->id);
  new_shopping_list.items = this->items;

  for (auto item : other.items){
    if (new_shopping_list.items.find(item.first) == new_shopping_list.items.end()){
      new_shopping_list.items[item.first] = item.second;
    }
    else{
      new_shopping_list.items[item.first] = new_shopping_list.items[item.first].merge(item.second);
      if (new_shopping_list.items[item.first].value() < 0){
        new_shopping_list.items[item.first].reset();
      }
    }
  }
  return new_shopping_list;
}

set<string> ShoppingList::get_items(){
  set<string> items;
  for (auto item : this->items){
    items.insert(item.first);
  }
  return items;
}

map<string, int> ShoppingList::get_items_with_quantity(){
  map<string, int> items;
  for (auto item : this->items){
    items[item.first] = item.second.value();
  }
  return items;
}

string ShoppingList::get_id(){
  return this->id;
}

string ShoppingList::print(){
  string output = "{ ";
  for (auto item : this->items){
    output += item.first + ": " + to_string(item.second.value()) + ", ";
  }
  output += "}";
  return output;
}

CRDTCounterMap ShoppingList::get_items_with_counter(){
  return this->items;
}

// int main(){
//   int user_id = 1;
//   int user_id2 = 2;
//   ShoppingList shopping_list("1");
//   shopping_list.set_user_id(user_id);

//   shopping_list.add("apple");
//   shopping_list.add("banana");
//   shopping_list.add("apple", 3);

//   cout << shopping_list.print() << endl
//        << endl;

//   ShoppingList shopping_list_copy = shopping_list.copy();
//   shopping_list_copy.set_user_id(user_id2);
//   shopping_list_copy.add("orange");
//   shopping_list_copy.add("apple", 2);
//   shopping_list_copy.add("banana", 20);
//   shopping_list_copy.decrease("banana", 21);

//   shopping_list.decrease("apple");
//   shopping_list.decrease("banana", 2);

//   cout << shopping_list.print() << endl;
//   cout << shopping_list_copy.print() << endl;
//   cout << shopping_list.merge(shopping_list_copy).print() << endl;

// }
