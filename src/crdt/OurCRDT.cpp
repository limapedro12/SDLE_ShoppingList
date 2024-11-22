#include <map>;
#include "CausalHistories.cpp";

using namespace std;

template <typename T>

class CRDTCounter{
private:
    int user_id;
    map<pair<int, int>, pair<unsigned, unsigned> m;
    CausalHistories causalHistory;

public:
    CRDTCounter(){
        this->positive_counter = 0;
        this->negative_counter = 0;
        this->causalHistory = CausalHistories();
    }

    void set_user_id(int user_id){
        this->user_id = user_id;
    }

    void fresh(){
        causalHistory.add(user_id);
        int contextNum = causalHistory.get(user_id);
        m[{user_id, contextNum}] = {0, 0};
    }

    void update(pair<int, int> pair){
        if
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