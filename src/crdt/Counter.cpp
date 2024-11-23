#include <map>
#include <set>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include "CausalHistories.cpp"

using namespace std;

class CRDTCounter{
private:
    int user_id;
    // pair<user_id, context_num> pair<positive_counter, negative_counter>
    map<pair<int, int>, pair<unsigned, unsigned>> m;
    CausalHistories causalHistory;

public:
    CRDTCounter(){
        m = {};
        this->causalHistory = CausalHistories();
    }

    map<pair<int, int>, pair<unsigned, unsigned>> get_map(){
        return m;
    }

    void set_user_id(int user_id){
        this->user_id = user_id;
    }

    void reset(){
        this->m.clear();
    }

    int value(){
        return accumulate(m.begin(), m.end(), 0, [](int sum, const auto &p)
                          { return sum + p.second.first - p.second.second; });

        // iterative form
        // int sum = 0;
        // for (auto &p : m){
        //     sum += p.second.first - p.second.second;
        // }
        // return sum;
    }

    void incr(){
        this->update({1, 0});
    }

    void decr(){
        this->update({0, 1});
    }

    void incr(int n){
        this->update({n, 0});
    }

    void decr(int n){
        this->update({0, n});
    }

    void set_value(int n){
        int val = n - this->value();
        if (val > 0)
            this->incr(val);
        else if (val < 0)
            this->decr(-val);
    }

    CRDTCounter merge(CRDTCounter other){
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

    CRDTCounter copy(){
        CRDTCounter new_counter;
        new_counter.m = m;
        new_counter.causalHistory = causalHistory.copy();
        return new_counter;
    }

    void fresh(){
        causalHistory.add(user_id);
        int contextNum = causalHistory.get(user_id);
        m[{user_id, contextNum}] = {0, 0};
    }

    void update(pair<int, int> pair){
        if (m.find({user_id, causalHistory.get(user_id)}) == m.end())
            this->fresh();
        m[{user_id, causalHistory.get(user_id)}].first += pair.first;
        m[{user_id, causalHistory.get(user_id)}].second += pair.second;
    }
};

typedef map<string, CRDTCounter> CRDTCounterMap;

int main(){
    CRDTCounter counter;
    counter.incr(2);

    cout << counter.value() << endl;

    CRDTCounter counter2 = counter.copy();
    counter.fresh();

    counter.incr(3);

    counter2.reset();

    cout << counter.value() << endl;
    cout << counter2.value() << endl;
    cout << counter.merge(counter2).value() << endl;

    return 0;
}

//     public:

//         AddWinsAWSet(){
//             this->addSet = {};
//             this->causalHistory = causalHistories();
//         }

//         void add(int id, T element){
//             this->causalHistory.add(id);
//             if (this->contains(element)){
//                 this->getElement(element)->second.second = this->causalHistory.get(id);
//             }
//             else{
//                 this->addSet.insert({element, {id, this->causalHistory.get(id)}});
//             }
//         }

//         void remove(int id, T element){
//             this->causalHistory.add(id);
//             if(this->contains(element)){
//                 this->addSet.erase(this->getElement(element));
//             }
//         }

//         bool contains(T element){
//             for(auto it = this->addSet.begin(); it != this->addSet.end(); it++){
//                 if(it->first == element){
//                     return true;
//                 }
//             }
//             return false;
//         }

//         const pair<T, pair<int, int>>* getElement(T element) {
//             for (auto it = this->addSet.begin(); it != this->addSet.end(); ++it) {
//                 if (it->first == element) {
//                     return &(*it);
//                 }
//             }
//             return nullptr;
//         }

//         set<T> elements(){
//             set<T> elements;
//             for(auto it = this->addSet.begin(); it != this->addSet.end(); it++){
//                 elements.insert(it->first);
//             }
//             return elements;
//         }

//         void merge(AddWinsAWSet<T> set){
//             for(auto it = set.addSet.begin(); it != set.addSet.end(); it++){
//                 if(this->contains(it->first)){
//                     if(this->getElement(it->first)->second.second < it->second.second){
//                         this->add(it->second.first, it->first);
//                     }
//                 }
//                 else{
//                     this->add(it->second.first, it->first);
//                 }
//             }
//         }
// };