#include <iostream>
#include <cassert>
#include <iostream>
#include <vector>
#include <filesystem>
#include "../include/edods/bst.hpp"
using namespace edods;

void test_empty_tree() {
    BST<int> tree;
    assert(tree.empty());
    assert(tree.size() == 0);
    assert(tree.height() == 0);
    assert(!tree.contains(42));
    assert(!tree.minimum().has_value());
    assert(!tree.maximum().has_value());
    assert(tree.validate());
    
    // Test clear su vuoto
    tree.clear();
    assert(tree.empty());
}

void test_single_element() {
    BST<int> tree;
    assert(tree.insert(10));
    assert(!tree.empty());
    assert(tree.size() == 1);
    assert(tree.height() == 1);
    assert(tree.contains(10));
    assert(tree.minimum().value() == 10);
    assert(tree.maximum().value() == 10);
    assert(tree.validate());

    // Reinserimento duplicato
    assert(!tree.insert(10));
    assert(tree.size() == 1);

    // Rimozione unico elemento
    assert(tree.del(10));
    assert(tree.empty());
    assert(tree.size() == 0);
    assert(tree.height() == 0);
    assert(tree.validate());
}

void test_deletions_edge_cases() {
    BST<int> tree;
    // Costruzione albero:
    //        20
    //       /  \
    //     10    30
    //       \   /
    //       15 25
    std::vector<int> keys = {20, 10, 30, 15, 25};
    for (int k : keys) tree.insert(k);

    assert(tree.size() == 5);
    assert(tree.validate());

    // 1. Eliminazione nodo con un solo figlio (10 ha solo figlio destro 15)
    assert(tree.del(10));
    assert(tree.validate());
    assert(!tree.contains(10));
    assert(tree.contains(15));
    assert(tree.size() == 4);

    // 2. Eliminazione radice con due figli (20 ha figli 15 e 30)
    assert(tree.del(20));
    assert(tree.validate());
    assert(!tree.contains(20));
    assert(tree.size() == 3);

    // 3. Eliminazione foglia (25)
    assert(tree.del(25));
    assert(tree.validate());
    assert(!tree.contains(25));
    assert(tree.size() == 2);
}

void test_persistence() {
    namespace fs = std::filesystem;
    fs::path temp_file = "test_tree.bin";

    {
        BST<int> tree;
        // Albero sbilanciato a zig-zag
        std::vector<int> data = {50, 20, 80, 10, 30, 70, 90, 25};
        for (int v : data) tree.insert(v);

        tree.save(temp_file);
    } // tree distrutto fuori dallo scope

    // Ricaricamento
    BST<int> loaded = BST<int>::load(temp_file);
    assert(loaded.validate());
    assert(loaded.size() == 8);
    assert(loaded.contains(50));
    assert(loaded.contains(25));
    assert(loaded.minimum().value() == 10);
    assert(loaded.maximum().value() == 90);

    // Test persistenza albero vuoto
    {
        BST<int> empty_tree;
        empty_tree.save(temp_file);
        BST<int> empty_loaded = BST<int>::load(temp_file);
        assert(empty_loaded.empty());
        assert(empty_loaded.size() == 0);
    }

    fs::remove(temp_file);
}

int main() {
    test_empty_tree();
    test_single_element();
    test_deletions_edge_cases();
    test_persistence();

    std::cout << "Tutti i test superati con successo.\n";
    return 0;
}