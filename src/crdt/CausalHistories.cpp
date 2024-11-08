#include <map>

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
    

};