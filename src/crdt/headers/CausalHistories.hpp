#include <map>
#include <iostream>

using namespace std;

class CausalHistories {

    private:

    map<int, int> causalHistory;

    void set_value(int id, int value);

    public:

    // Constructor
    CausalHistories();

    // add one event to the causal history in user id
    void add(int id);

    // get the value of the causal history in user id
    int get(int id);

    // get full map of the causal history
    map<int, int> getMap();

    // merge two causal histories
    CausalHistories merge(CausalHistories ch);

    // check if two causal histories are equal
    bool operator==(const CausalHistories &other) const;

    // check if one causal history happens before another causal history
    bool operator<(CausalHistories ch) const;

    // check if one pair user id and event number happpens after the current causal history
    bool operator<(pair<int, int> p) const;

    // copy the causal history
    CausalHistories copy() const;

    // represent the causal history as a string
    string print() const;
};
