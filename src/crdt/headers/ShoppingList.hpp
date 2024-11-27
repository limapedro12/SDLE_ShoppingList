#include "string"
#include "set"
#include "map"
#include "Counter.hpp"

class ShoppingList{
private:
  CRDTCounterMap items;
  string id;
  int user_id = 0;

public:
  ShoppingList(string id);

  void set_user_id(int user_id);

  void reset_user_id();

  void add(string item);

  void add(string item, int n);

  void decrease(string item);

  void decrease(string item, int n);

  void set_value(string item, int value);

  void reset(string item);

  int get_quantity(string item);

  bool is_item_present(string item);

  ShoppingList copy();

  void fresh();

  ShoppingList merge(ShoppingList other);

  set<string> get_items();

  map<string, int> get_items_with_quantity();
  
  string get_id();

  string print();
};
