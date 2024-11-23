#include <string>
#include <iostream>
#include <vector>
#include "ShoppingList.cpp"

using namespace std;

void add(ShoppingList& shopping_list){
  string item;
  int quantity;
  cin >> quantity >> item;

  shopping_list.add(item, quantity);

  cout << "Added " << quantity << " " << item << " to the shopping list" << endl;
}

void decrease(ShoppingList& shopping_list){
  string item;
  int quantity;
  cin >> quantity >> item;
  int original_quantity = shopping_list.get_quantity(item);
  shopping_list.decrease(item, quantity);

  if (original_quantity < quantity)
    cout << "Decresed " << original_quantity << " " << item << " from the shopping list" << endl;
  else
    cout << "Decreased " << quantity << " " << item << " from the shopping list" << endl;
}

void reset(ShoppingList &shopping_list){
  string item;
  cin >> item;
  shopping_list.reset(item);
  cout << "Reset " << item << " from the shopping list" << endl;
}

int base_id = 0;
string generate_id(){
  return to_string(base_id++);
}

void remove(vector<ShoppingList> &shopping_lists){
  string id;
  cin >> id;
  for (int i = 0; i < shopping_lists.size(); i++)
  {
    if (shopping_lists[i].get_id() == id)
    {
      shopping_lists.erase(shopping_lists.begin() + i);
      cout << "Removed shopping list with id " << id << endl;
      return;
    }
  }
}

ShoppingList *get(vector<ShoppingList> &shopping_lists){
  string id;
  cin >> id;
  for (auto& shopping_list : shopping_lists){
    if (shopping_list.get_id() == id)
      return &shopping_list;
  }
  return nullptr;
}

void list(vector<ShoppingList> &shopping_lists){
  for (auto shopping_list : shopping_lists)
    cout << "Id: " << shopping_list.get_id() << endl;
}


int main(){
  vector<ShoppingList> shopping_lists;
  ShoppingList *current_shopping_list = nullptr;
  while(true){
    string command;
    cin >> command;
    if (command == "exit")
      break;
    else if (command == "create"){
      shopping_lists.push_back(ShoppingList(generate_id()));
      cout << "Created a new shopping list" << endl;
    } else if (command == "remove")
      remove(shopping_lists);
    else if (command == "list")
      list(shopping_lists);
    else if (command == "select"){
      current_shopping_list = get(shopping_lists);
      cout << "Selected shopping list with id " << current_shopping_list->get_id() << endl;
    } 
    else if (command == "leave")
      current_shopping_list = nullptr;
    else if (command == "add")
      add(*current_shopping_list);
    else if (command == "decrease")
      decrease(*current_shopping_list);
    else if (command == "reset")
      reset(*current_shopping_list);
    else if (command == "print")
      cout << (*current_shopping_list).print() << endl;
    else
      cout << "Invalid command" << endl;
  }
}