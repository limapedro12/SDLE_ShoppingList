
#include "string"
#include "set"
#include "map"
#include "../../libs/json.hpp"
#include "Counter.hpp"

using namespace std;
using json = nlohmann::json;

#ifndef COUNTERMAP_H
#define COUNTERMAP_H

struct CounterMapInstance{
  CRDTCounter counter;
  string user_id;
  int context_num;
};

class CounterMap{
private:
  CausalHistories counter_histories;
  map<string, CounterMapInstance> items;

public:
  CounterMap();

  CounterMap(json j);

  void add(string item, string user_id);

  void add(string item, int n, string user_id);

  void decrease(string item, string user_id);

  void decrease(string item, int n, string user_id);

  void set_value(string item, int value, string user_id);

  void remove(string item, string user_id);

  int get_quantity(string item);

  CRDTCounter operator[](string item);

  bool contains(string item);

  CounterMap copy();

  void fresh(string user_id);

  CounterMap merge(CounterMap other, string user_id);

  set<string> get_items();

  map<string, int> get_items_with_quantity();

  string print();

  json toJSON();
};

#endif


