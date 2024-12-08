#include "../headers/CLI.hpp"

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

void set_value(ShoppingList &shopping_list){
  string item;
  int value;
  cin >> value >> item;
  shopping_list.set_value(item, value);
  cout << "Set " << item << " to " << value << endl;
}

void reset(ShoppingList &shopping_list){
  string item;
  cin >> item;
  shopping_list.remove(item);
  cout << "Remove " << item << " from the shopping list" << endl;
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

void create(vector<ShoppingList> &shopping_lists){
  ShoppingList shopping_list(generate_id());
  shopping_list.setUserId("1");
  shopping_lists.push_back(shopping_list);
  cout << "Created a new shopping list" << endl;
}

void push(ShoppingList &shopping_list){
  shopping_list.fresh();

  // send to server

  cout << "Push not implemented yet" << endl;
}

void pull(ShoppingList &shopping_list){
  // receive from server
  cout << "Pull not implemented yet" << endl;
}


int terminal(){
  vector<ShoppingList> shopping_lists;
  ShoppingList *current_shopping_list = nullptr;
  while(true){
    if (current_shopping_list == nullptr)
      cout << "> ";
    else
      cout << "[" << current_shopping_list->get_id() << "] > ";
    string command;
    cin >> command;
    if (command == "exit")
      break;
    else if (command == "create")
      create(shopping_lists);
    else if (command == "remove")
      remove(shopping_lists);
    else if (command == "list")
      list(shopping_lists);
    else if (command == "select"){
      current_shopping_list = get(shopping_lists);
      cout << "Selected shopping list with id " << current_shopping_list->get_id() << endl;
    } 
    else if (command == "leave")
      current_shopping_list = nullptr;
    else if (command == "add"){
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        add(*current_shopping_list);
    } else if (command == "decrease"){
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        decrease(*current_shopping_list);
    } else if (command == "set_value") {
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        set_value(*current_shopping_list);
    } else if (command == "reset") {
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        reset(*current_shopping_list);
    } else if (command == "print") {
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        cout << (*current_shopping_list).print() << endl;
    } else if (command == "push")
    {
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        push(*current_shopping_list);
    }
    else if (command == "pull")
    {
      if (current_shopping_list == nullptr)
        cout << "No shopping list selected" << endl;
      else
        pull(*current_shopping_list);
    } else {
      cout << "Invalid command" << endl;
    }
  }
  return 0;
}