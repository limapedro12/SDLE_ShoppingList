#include "../crdt/headers/ShoppingList.hpp"

int testShoppingList(){
  string user_id = "1";
  string user_id2 = "2";
  ShoppingList shopping_list("1");
  shopping_list.setUserId(user_id);

  cout << "Original shopping list: " << endl;
  shopping_list.add("apple");
  shopping_list.add("banana", 2);
  shopping_list.add("apple", 3);
  shopping_list.add("pinapple");
  shopping_list.add("kiwi");
  shopping_list.add("mango");

  // add 2 apple 2 banana 2 pinapple

  cout << shopping_list.print() << endl
       << endl;

  ShoppingList shopping_list_copy = shopping_list.copy();
  shopping_list_copy.setUserId(user_id2);
  shopping_list_copy.add("orange");
  shopping_list_copy.add("apple", 2);
  shopping_list_copy.add("banana", 20);
  shopping_list_copy.decrease("banana", 21);
  shopping_list_copy.remove("pinapple");
  shopping_list_copy.remove("mango");

  // decrease 1 apple remove banana add 1 orange

  shopping_list.decrease("apple");
  shopping_list.decrease("banana", 2);
  shopping_list.add("pinapple");

  // add 1 apple 1 banana change pinapple 10

  cout << "Shopping list after user 1:" << endl;
  cout << shopping_list.print() << endl
       << endl;

  cout << "Shopping list after user 2:" << endl;
  cout << shopping_list_copy.print() << endl
       << endl;

  // cout << shopping_list_copy.contentsToJSON().dump(4) << endl;
  ShoppingList merged_shopping_list = shopping_list.merge(shopping_list_copy);

  cout << "Merged shopping list:" << endl;
  cout << merged_shopping_list.print() << endl
       << endl;

  assert(merged_shopping_list.get_quantity("apple") == 5); 

  assert(merged_shopping_list.get_quantity("banana") == 0);

  assert(merged_shopping_list.get_quantity("pinapple") == 1);

  assert(merged_shopping_list.contains("orange") == true);

  assert(merged_shopping_list.contains("mango") == false);

  assert(merged_shopping_list.get_quantity("kiwi") == 1);

  ShoppingList dummy_shopping_list("123", merged_shopping_list.contentsToJSON());

  assert(dummy_shopping_list.get_quantity("apple") == 5);

  return 0;
}


int main(){
  if (testShoppingList() == 0){
    cout << "All ShoppingList tests passed" << endl;
  } else {
    cout << "Some ShoppingList tests failed" << endl;
  }

  return 0;
}
