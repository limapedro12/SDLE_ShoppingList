#include "string"
#include "set"
#include "map"
#include "../../libs/json.hpp"
#include "Counter.hpp"

using namespace std;
using json = nlohmann::json;

class ShoppingList{
private:
  CRDTCounterMap items;
  string id;
  string user_id = "";

public:
  ShoppingList(string id);

  ShoppingList(string id, json j);

  void setUserId(string user_id);

  void resetUserId();

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

  CRDTCounterMap get_items_with_counter();
  
  string get_id();

  string print();

  json contentsToJSON();

};
