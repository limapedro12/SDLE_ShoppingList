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
    map<pair<string, int>, pair<unsigned, unsigned>> m;
    CausalHistories causalHistory;

public:
    CRDTCounter();

    map<pair<string, int>, pair<unsigned, unsigned>> get_map();

    void reset();

    int value();

    void incr(string user_id);

    void decr(string user_id);

    void incr(int n, string user_id);

    void decr(int n, string user_id);

    void set_value(int n, string user_id);

    CRDTCounter merge(CRDTCounter other);

    CRDTCounter copy();

    void fresh(string user_id);

    void update(pair<int, int> pair, string user_id);

    CausalHistories get_causal_history();
};

typedef map<string, CRDTCounter> CRDTCounterMap;
