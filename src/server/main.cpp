#include "Server.cpp"
#include <iostream>

int main(){
    Server server;
    std::cout << "Insert user id: ";
    int id;
    std::cin >> id;
    server.getShoppingList(id);
    return 0;
}