#include <map>
#include <iostream>

using namespace std;

class CausalHistories {

    private:

    map<int, int> causalHistory;

    public:

    // Constructor
    CausalHistories(){
        this->causalHistory = {};
    }

    // pass the id to incrment the value of the id in the causal history, if the key does not exist, it will be created and the value will be set to 1
    void add(int id){
        if(this->causalHistory.find(id) == this->causalHistory.end()){
            this->causalHistory[id] = 1;
        } else {
            this->causalHistory[id] += 1;
        }
    }

    // give the id and get the key value pair from the causal history
    int get(int id){
        if(this->causalHistory.find(id) == this->causalHistory.end()){
            return 0;
        } else {
            return this->causalHistory[id];
        }
    }

    map<int, int> getMap(){
        return causalHistory;
    }

    void merge(CausalHistories ch){
        auto my_it = causalHistory.begin();
        auto ch_it = ch.getMap().begin();

        while(my_it != causalHistory.end() && ch_it != ch.getMap().end()){
            if (my_it->first == ch_it->first){
                if (my_it->second < ch_it->second){
                    my_it->second = ch_it->second;
                }
                my_it++;
                ch_it++;
            } else if (my_it->first < ch_it->first){
                my_it++;
            } else {
                causalHistory.insert({ch_it->first, ch_it->second});
                ch_it++;
            }
        }
    }

    bool operator==(const CausalHistories &other) const{
        return causalHistory == other.causalHistory;
    }

    bool operator<(CausalHistories ch) const{
        auto my_it = causalHistory.begin();
        auto ch_it = ch.getMap().begin();

        while (true){
            bool found_higher = false;
            if(my_it == causalHistory.end())
                return found_higher;
            else if (ch_it == ch.getMap().end())
                return true;
            else if (my_it->first == ch_it->first){
                if (my_it->second < ch_it->second)
                    found_higher = true;
                if (my_it->second > ch_it->second)
                    return false;
                my_it++;
                ch_it++;
            } else if (my_it->first < ch_it->first)
                return false;
            else
                my_it++;
        }
    }

    CausalHistories copy(){
        CausalHistories ch;
        ch.causalHistory = causalHistory;
        return ch;
    }

    string print(){
        string s = "";
        for (auto it = causalHistory.begin(); it != causalHistory.end(); it++)
        {
            s += to_string(it->first) + ":" + to_string(it->second) + " ";
        }
        return s;
    }
};


int main() {
    CausalHistories ch1;
    ch1.add(1);
    ch1.add(2);


    CausalHistories ch2;
    ch2.add(1);
    ch2.add(2);

    CausalHistories ch1_copy = ch1.copy();
    ch1_copy.add(1);

    cout << ch1.print() << endl;
    cout << ch1_copy.print() << endl;
    cout << ch2.print() << endl;

    cout << (bool)(ch1 == ch2) << endl;
    cout << (bool)(ch1 < ch1_copy) << endl;

}