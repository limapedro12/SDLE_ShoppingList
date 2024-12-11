#include "../libs/zhelpers.hpp"
#include <queue>
#include "../crdt/headers/ShoppingList.hpp"

using namespace std;

int receiveSubscriptions(zmq::socket_t &subscriber, vector<ShoppingList>& shopping_lists);
