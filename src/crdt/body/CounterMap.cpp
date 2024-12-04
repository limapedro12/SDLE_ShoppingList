#include "../headers/CounterMap.hpp"

CounterMap::CounterMap(){
  this->counter_histories = CausalHistories();
  this->items = map<string, CounterMapInstance>();
}

CounterMap::CounterMap(json j){
  this->counter_histories = CausalHistories(j["causal_histories"]);
  this->items = map<string, CounterMapInstance>();
  for (auto item : j.items()){
    this->items[item.key()] = {CRDTCounter(item.value()["counter"]), item.value()["user_id"], item.value()["context_num"]};
  }
}

json CounterMap::toJSON(){
  json j;
  j["causal_histories"] = this->counter_histories.toJSON();
  for (auto item : this->items)
  {
    j[item.first] = {
        {"counter", item.second.counter.toJSON()},
        {"user_id", item.second.user_id},
        {"context_num", item.second.context_num}};
  }
  return j;
}

void CounterMap::add(string item, string user_id){
  if (this->items.find(item) == this->items.end())
  {
    this->items[item] = {CRDTCounter(), user_id, this->counter_histories.get(user_id)};
  }
  this->items[item].counter.incr(user_id);
  this->counter_histories.add(user_id);
}

void CounterMap::add(string item, int n, string user_id)
{
  this->counter_histories.add(user_id);
  if (this->items.find(item) == this->items.end())
  {
    this->items[item] = {CRDTCounter(), user_id, this->counter_histories.get(user_id)};
  }
  this->items[item].counter.incr(n, user_id);
}

void CounterMap::decrease(string item, string user_id)
{
  if (this->items.find(item) == this->items.end())
  {
    return;
  }
  this->counter_histories.add(user_id);
  this->items[item].counter.decr(user_id);
  this->items[item].user_id = user_id;
  this->items[item].context_num = this->counter_histories.get(user_id);
}

void CounterMap::decrease(string item, int n, string user_id){
  if (this->items.find(item) == this->items.end())
  {
    return;
  }
  this->counter_histories.add(user_id);
  this->items[item].counter.decr(n, user_id);
  this->items[item].user_id = user_id;
  this->items[item].context_num = this->counter_histories.get(user_id);
}

void CounterMap::set_value(string item, int value, string user_id){
  if (this->items.find(item) == this->items.end())
  {
    this->items[item] = {CRDTCounter(), user_id, this->counter_histories.get(user_id)};
  }
  this->counter_histories.add(user_id);
  this->items[item].counter.set_value(value, user_id);
  this->items[item].user_id = user_id;
  this->items[item].context_num = this->counter_histories.get(user_id);
}

void CounterMap::remove(string item, string user_id){
  if (this->items.find(item) == this->items.end())
  {
    return;
  }
  this->counter_histories.add(user_id);
  this->items[item].counter.reset();
  this->items[item].user_id = user_id;
  this->items[item].context_num = this->counter_histories.get(user_id);
}

int CounterMap::get_quantity(string item){
  if (this->items.find(item) == this->items.end())
  {
    return 0;
  }
  return this->items[item].counter.value();
}

CRDTCounter CounterMap::operator[](string item){
  if (this->items.find(item) == this->items.end()){
    throw std::invalid_argument("item not found");
  }
  return this->items[item].counter;
}

bool CounterMap::contains(string item){
  return (this->items.find(item) != this->items.end()) || (this->get_quantity(item) == 0);
}

CounterMap CounterMap::copy(){
  CounterMap new_counter_map;
  new_counter_map.counter_histories = this->counter_histories.copy();
  new_counter_map.items = this->items;
  return new_counter_map;
}

void CounterMap::fresh(string user_id){
  for (auto &item : this->items) {
    item.second.counter.fresh(user_id);
  }
}

CounterMap CounterMap::merge(CounterMap other, string userId){
  CounterMap new_counter_map;
  new_counter_map.counter_histories = this->counter_histories.merge(other.counter_histories);
  map<string, CounterMapInstance> new_items;

  for (auto &item : this->items){
    if (other.items.find(item.first) == other.items.end())
      new_items[item.first] = item.second; 
    else {
      pair<string, int> causalThis = {item.second.user_id, item.second.context_num};
      pair<string, int> other_causal = {other.items[item.first].user_id, other.items[item.first].context_num};
      if (causalThis == other_causal)
        new_items[item.first] = item.second;
      // else if (this->counter_histories < other_causal)
      //   new_items[item.first] = other.items[item.first];
      // else if (other.counter_histories < causalThis)
      //   new_items[item.first] = item.second;
      else
        new_items[item.first] = {item.second.counter.merge(other.items[item.first].counter), userId, this->counter_histories.get(userId)};
    }
  }

  for (auto &item : other.items){
    if (this->items.find(item.first) == this->items.end())
      new_items[item.first] = item.second;
  }

  new_counter_map.items = new_items;

  return new_counter_map;
}

set<string> CounterMap::get_items(){
  set<string> items;
  for (auto &item : this->items)
    items.insert(item.first);
  return items;
}

map<string, int> CounterMap::get_items_with_quantity(){
  map<string, int> items;
  for (auto &item : this->items)
    items[item.first] = item.second.counter.value();
  return items;
}

string CounterMap::print(){
  string result = "{ ";
  for (auto &item : this->items)
    result += item.first + ": " + to_string(item.second.counter.value()) + ", ";
  result += " }";
  return result;
}
