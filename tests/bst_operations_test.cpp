#include <iostream>
#include "edods/bst.hpp"

int main(){
    edods::BST<int> test;
    std::cout << "l'albero è vuoto ? ";
    std::cout << test.empty();

    return 0;
     
}