#include <map>
#include <set>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include "CausalHistories.hpp"

using namespace std;

class CRDTCounter{
private:
    // pair<user_id, context_num> pair<positive_counter, negative_counter>
    map<pair<int, int>, pair<unsigned, unsigned>> m;
    CausalHistories causalHistory;

public:
    CRDTCounter();

    map<pair<int, int>, pair<unsigned, unsigned>> get_map();

    void reset();

    int value();

    void incr(int user_id);

    void decr(int user_id);

    void incr(int n, int user_id);

    void decr(int n, int user_id);

    void set_value(int n, int user_id);

    CRDTCounter merge(CRDTCounter other);

    CRDTCounter copy();

    void fresh(int user_id);

    void update(pair<int, int> pair, int user_id);

    CausalHistories get_causal_history();
};

typedef map<string, CRDTCounter> CRDTCounterMap;
