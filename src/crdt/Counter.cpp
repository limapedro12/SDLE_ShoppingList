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

    void reset(){
        this->m.clear();
    }

    int value(){
        return accumulate(m.begin(), m.end(), 0, [](int sum, const auto &p)
                          { return sum + p.second.first - p.second.second; });
    }

    void incr(int user_id){
        this->update({1, 0}, user_id);
    }

    void decr(int user_id){
        this->update({0, 1}, user_id);
    }

    void incr(int n, int user_id){
        this->update({n, 0}, user_id);
    }

    void decr(int n, int user_id){
        this->update({0, n}, user_id);
    }

    void set_value(int n, int user_id){
        int val = n - this->value();
        if (val > 0)
            this->incr(val, user_id);
        else if (val < 0)
            this->decr(-val, user_id);
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

    void fresh(int user_id){
        causalHistory.add(user_id);
        int contextNum = causalHistory.get(user_id);
        m[{user_id, contextNum}] = {0, 0};
    }

    void update(pair<int, int> pair, int user_id){
        if (m.find({user_id, causalHistory.get(user_id)}) == m.end())
            this->fresh(user_id);
        m[{user_id, causalHistory.get(user_id)}].first += pair.first;
        m[{user_id, causalHistory.get(user_id)}].second += pair.second;
    }
};

typedef map<string, CRDTCounter> CRDTCounterMap;

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
