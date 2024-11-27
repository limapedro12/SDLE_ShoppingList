#include "../headers/Counter.hpp"

using namespace std;


CRDTCounter::CRDTCounter(){
    m = {};
    this->causalHistory = CausalHistories();
}

map<pair<string, int>, pair<unsigned, unsigned>> CRDTCounter::get_map(){
    return m;
}

void CRDTCounter::reset(){
    this->m.clear();
}

int CRDTCounter::value(){
    return accumulate(m.begin(), m.end(), 0, [](int sum, const auto &p)
                        { return sum + p.second.first - p.second.second; });
}

void CRDTCounter::incr(string user_id){
    this->update({1, 0}, user_id);
}

void CRDTCounter::decr(string user_id){
    this->update({0, 1}, user_id);
}

void CRDTCounter::incr(int n, string user_id){
    this->update({n, 0}, user_id);
}

void CRDTCounter::decr(int n, string user_id){
    this->update({0, n}, user_id);
}

void CRDTCounter::set_value(int n, string user_id){
    int val = n - this->value();
    if (val > 0)
        this->incr(val, user_id);
    else if (val < 0)
        this->decr(-val, user_id);
}

CRDTCounter CRDTCounter::merge(CRDTCounter other){
    CRDTCounter new_counter;
    auto other_map = other.get_map();

    for(auto val: m){
        auto other_pair = other_map.find(val.first);
        if (other_pair != other_map.end()){
            pair<int, int> pair = {
                max(val.second.first, other_pair->second.first),
                max(val.second.second, other_pair->second.second)
            };
            new_counter.m[val.first] = pair;
        }
        else if (other.causalHistory < val.first){
            new_counter.m[val.first] = val.second;
        }
    }

    for (auto val : other_map){
        if (m.find(val.first) == m.end() && causalHistory < val.first){
            new_counter.m[val.first] = val.second;
        }
    }

    new_counter.causalHistory = causalHistory.merge(other.causalHistory);

    return new_counter;
}

CRDTCounter CRDTCounter::copy(){
    CRDTCounter new_counter;
    new_counter.m = m;
    new_counter.causalHistory = causalHistory.copy();
    return new_counter;
}

void CRDTCounter::fresh(string user_id){
    causalHistory.add(user_id);
    int contextNum = causalHistory.get(user_id);
    m[{user_id, contextNum}] = {0, 0};
}

void CRDTCounter::update(pair<int, int> pair, string user_id){
    if (m.find({user_id, causalHistory.get(user_id)}) == m.end())
        this->fresh(user_id);
    m[{user_id, causalHistory.get(user_id)}].first += pair.first;
    m[{user_id, causalHistory.get(user_id)}].second += pair.second;
}

CausalHistories CRDTCounter::get_causal_history(){
    return causalHistory;
}

json CRDTCounter::toJSON(){
    json json;
    json["causalHistory"] = causalHistory.toJSON();
    json["counter"] = m;
    return json;
}

CRDTCounter::CRDTCounter(json j){
    causalHistory = CausalHistories(j["causalHistory"]);
    m = j["counter"];
}

// int main(){
//     int user_id = 1;
//     int user_id2 = 2;
//     CRDTCounter counter;
//     counter.incr(2, user_id);

//     cout << counter.value() << endl;

//     CRDTCounter counter2 = counter.copy();
//     counter.fresh(user_id);

//     counter.incr(3, user_id);

//     counter2.reset();

//     cout << counter.value() << endl;
//     cout << counter2.value() << endl;
//     cout << counter.merge(counter2).value() << endl;

//     return 0;
// }
