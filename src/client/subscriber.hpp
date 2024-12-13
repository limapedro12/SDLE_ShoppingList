#include "../libs/zhelpers.hpp"
#include <map>
#include <fstream>
#include <mutex>
#include "../crdt/headers/ShoppingList.hpp"

using namespace std;

void receiveSubscriptions(zmq::socket_t &subscriber, string userId);
ShoppingList getNewVersion(string list_id);

