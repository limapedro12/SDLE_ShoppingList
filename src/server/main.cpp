#include "Server.cpp"
#include <iostream>
#include "md5.cpp"

int main() {
    Server server;
    md5 encrypter;
    std::cout << "Insert user id: ";
    int id;
    std::cin >> id;

    // Encrypt the ID and use the resulting hash as a string
    std::string hashed_id = encrypter.encrypt(std::to_string(id));
    std::cout << "Hashed user id: " << hashed_id << std::endl;

    server.getShoppingList(hashed_id);

    std::cout << "Insert item to add to the list: ";
    std::string item;
    std::cin >> item;

    std::cout << "Insert quantity: ";
    int quantity;
    std::cin >> quantity;

    server.updateShoppingList(hashed_id, item, quantity);

    return 0;
}
