#include "../libs/zhelpers.hpp"
#include <map>
#include <fstream>
#include <mutex>
#include <atomic>
#include "../crdt/headers/ShoppingList.hpp"

using namespace std;

void receiveSubscriptions(zmq::socket_t &subscriber, string userId, std::atomic<bool> &running);
ShoppingList getNewVersion(string list_id);

