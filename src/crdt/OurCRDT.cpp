#include <map>;
#include <set>;
#include <iostream>;
#include <numeric>;
#include <algorithm>;
#include "CausalHistories.cpp";

using namespace std;

template <typename T>

class CRDTCounter{
private:
    int user_id;
    // pair<user_id, context_num> pair<positive_counter, negative_counter>
    map<pair<int, int>, pair<unsigned, unsigned> m;
    CausalHistories causalHistory;

public:
    CRDTCounter(){
        this->positive_counter = 0;
        this->negative_counter = 0;
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
        return reduce(m.begin(), m.end(), 0, [](int sum, const auto &p)
                      { return sum + p.second.first - p.second.second; });
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
                }
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

    void fresh(){
        causalHistory.add(user_id);
        int contextNum = causalHistory.get(user_id);
        m[{user_id, contextNum}] = {0, 0};
    }

    void update(pair<int, int> pair){
        if (m.find({usei_id, causalHistory.get(user_id)}) == m.end())
            this->fresh();
        m[{user_id, causalHistory.get(user_id)}].first += pair.first;
        m[{user_id, causalHistory.get(user_id)}].second += pair.second;
    }
}

typedef map<string, CRDTCounter> crdtMap OurCRDT;

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