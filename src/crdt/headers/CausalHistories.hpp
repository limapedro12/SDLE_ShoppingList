#include <map>
#include <string>
#include <iostream>
#include "../../libs/json.hpp"

using namespace std;
using json = nlohmann::json;

class CausalHistories {

    private:

    map<string, int> causalHistory;

    void set_value(string id, int value);

    public:

    // Constructor
    CausalHistories();

    // Constructor from  json
    CausalHistories(json j);

    // add one event to the causal history in user id
    void add(string id);

    // get the value of the causal history in user id
    int get(string id);

    // get full map of the causal history
    map<string, int> getMap();

    // merge two causal histories
    CausalHistories merge(CausalHistories ch);

    // check if two causal histories are equal
    bool operator==(const CausalHistories &other) const;

    // check if one causal history happens before another causal history
    bool operator<(CausalHistories ch) const;

    // check if one pair user id and event number happpens after the current causal history
    bool operator<(pair<string, int> p) const;

    // copy the causal history
    CausalHistories copy() const;

    // represent the causal history as a string
    string print() const;

    // return teh equivalent json object
    json toJSON();
};
