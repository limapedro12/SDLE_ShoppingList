#include <set>;
#include "CausalHistories.cpp";

using namespace std;

template <typename T>
class AddWinsAWSet{

    private:

        set<pair<T, pair<int, int>>> addSet;
        CausalHistories causalHistory;

    public:

        AddWinsAWSet(){
            this->addSet = {};
            this->causalHistory = causalHistories();
        }

        void add(int id, T element){
            this->causalHistory.add(id);
            if (this->contains(element)){
                this->getElement(element)->second.second = this->causalHistory.get(id);
            }
            else{
                this->addSet.insert({element, {id, this->causalHistory.get(id)}});
            }
        }

        void remove(int id, T element){
            this->causalHistory.add(id);
            if(this->contains(element)){
                this->addSet.erase(this->getElement(element));
            }
        }

        bool contains(T element){
            for(auto it = this->addSet.begin(); it != this->addSet.end(); it++){
                if(it->first == element){
                    return true;
                }
            }
            return false;
        }

        const pair<T, pair<int, int>>* getElement(T element) {
            for (auto it = this->addSet.begin(); it != this->addSet.end(); ++it) {
                if (it->first == element) {
                    return &(*it);
                }
            }
            return nullptr;
        }

        set<T> elements(){
            set<T> elements;
            for(auto it = this->addSet.begin(); it != this->addSet.end(); it++){
                elements.insert(it->first);
            }
            return elements;
        }

        void merge(AddWinsAWSet<T> set){
            for(auto it = set.addSet.begin(); it != set.addSet.end(); it++){
                if(this->contains(it->first)){
                    if(this->getElement(it->first)->second.second < it->second.second){
                        this->add(it->second.first, it->first);
                    }
                }
                else{
                    this->add(it->second.first, it->first);
                }
            }
        }
};