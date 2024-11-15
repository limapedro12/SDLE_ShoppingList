#include <iostream>
#include <fstream>
#include <set>

using namespace std;

class Server{
    public:

    // gets the user's shopping list from the server, the list is in a hashed txt file in the lists folder and is fetched by the user's id, and printed to the console
    void getShoppingList(int id){
        string path = "../../lists/" + to_string(id) + ".txt";
        ifstream file(path);
        string line;
        while(getline(file, line)){
            cout << line << endl;
        }
    }
};