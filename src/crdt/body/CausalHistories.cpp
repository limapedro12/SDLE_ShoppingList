#include "../headers/CausalHistories.hpp"

using namespace std;


void CausalHistories::set_value(string id, int value){
    this->causalHistory[id] = value;
}

// Constructor
CausalHistories::CausalHistories(){
    this->causalHistory = {};
}

void CausalHistories::add(string id){
    if(this->causalHistory.find(id) == this->causalHistory.end()){
        this->causalHistory[id] = 1;
    } else {
        this->causalHistory[id] += 1;
    }
}

int CausalHistories::get(string id){
    if(this->causalHistory.find(id) == this->causalHistory.end()){
        return 0;
    } else {
        return this->causalHistory[id];
    }
}

map<string, int> CausalHistories::getMap(){
    return causalHistory;
}

CausalHistories CausalHistories::merge(CausalHistories ch){
    CausalHistories this_copy = *this;
    auto my_it = this_copy.getMap().begin();
    auto ch_it = ch.getMap().begin();

    auto this_map = this_copy.getMap();
    auto ch_map = ch.getMap();

    for (auto val : ch_map){
        if (this_map.find(val.first) == this_map.end())
            this_copy.set_value(val.first, val.second);
        else if (this_map[val.first] < val.second)
            this_copy.set_value(val.first, val.second);
    }

    return this_copy;
}

bool CausalHistories::operator==(const CausalHistories &other) const{
    return causalHistory == other.causalHistory;
}

bool CausalHistories::operator<(CausalHistories ch) const{
    auto my_it = causalHistory.begin();
    auto ch_it = ch.getMap().begin();

    bool found_higher = false;
    while (true){
        if (ch_it == ch.getMap().end())
            return false;
        else if(my_it == causalHistory.end()){
            cout << "my_it == causalHistory.end()" << endl;
            return found_higher;
        } else if (my_it->first == ch_it->first) {
            if (my_it->second < ch_it->second)
                found_higher = true;
            else if (my_it->second > ch_it->second)
                return false;
            my_it++;
            ch_it++;
        } else if (my_it->first < ch_it->first){
            return false;
        } else
            my_it++;
    }
}

bool CausalHistories::operator<(pair<string, int> p) const{
    return (causalHistory.find(p.first) == causalHistory.end()) || 
            (causalHistory.find(p.first)->second < p.second);
}

CausalHistories CausalHistories::copy() const{
    CausalHistories ch;
    ch.causalHistory = causalHistory;
    return ch;
}

string CausalHistories::print() const{
    string s = "";
    for (auto it = causalHistory.begin(); it != causalHistory.end(); it++){
        s += it->first + ":" + to_string(it->second) + " ";
    }
    return s;
}

json CausalHistories::toJSON(){
    return causalHistory;
}

CausalHistories::CausalHistories(json j){
    causalHistory = j;
}

// int main() {
//     CausalHistories ch1;
//     ch1.add(1);
//     ch1.add(2);
//     ch1.add(5);


//     CausalHistories ch2;
//     ch2.add(1);
//     ch2.add(2);
//     ch2.add(3);

//     CausalHistories ch1_copy = ch1.copy();
//     ch1_copy.add(1);

//     cout << ch1.print() << endl;
//     cout << ch1_copy.print() << endl;
//     cout << ch2.print() << endl;

//     cout << (bool)(ch1 == ch2) << endl;
//     cout << (bool)(ch2 < ch1) << endl;

//     CausalHistories ch3 = ch1.merge(ch2);
//     cout << ch3.print() << endl;

// }