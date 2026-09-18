#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>
#include <cmath>
#include "../include/edods/rb.hpp"

using namespace edods;

void test_empty_tree() {
    RB<int> tree;
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
    RB<int> tree;
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
    RB<int> tree;
    // Costruzione albero:
    // Inseriamo vari nodi per testare i diversi casi di cancellazione e rotazione
    std::vector<int> keys = {20, 10, 30, 15, 25, 5, 35};
    for (int k : keys) {
        assert(tree.insert(k));
        assert(tree.validate());
    }

    assert(tree.size() == 7);
    assert(tree.validate());

    // 1. Eliminazione foglia
    assert(tree.del(5));
    assert(tree.validate());
    assert(!tree.contains(5));
    assert(tree.size() == 6);

    // 2. Eliminazione nodo con figli e ri-bilanciamento
    assert(tree.del(10));
    assert(tree.validate());
    assert(!tree.contains(10));
    assert(tree.contains(15));
    assert(tree.size() == 5);

    // 3. Eliminazione radice con due figli
    assert(tree.del(20));
    assert(tree.validate());
    assert(!tree.contains(20));
    assert(tree.size() == 4);

    // 4. Eliminazione sequenziale di tutti i restanti nodi
    for (int remaining : {15, 25, 30, 35}) {
        assert(tree.del(remaining));
        assert(tree.validate());
    }

    assert(tree.empty());
    assert(tree.size() == 0);
    assert(tree.height() == 0);
}

void test_balance_and_rotations() {
    RB<int> tree;

    // Inserimento ordinato crescente:
    // In un BST normale questo creerebbe una lista degenere di altezza N (O(n)).
    // Nel Red-Black Tree l'albero si autobilancia e l'altezza rimane rigidamente O(log n).
    const int n = 50;
    for (int i = 1; i <= n; ++i) {
        assert(tree.insert(i));
        assert(tree.validate());
    }

    assert(tree.size() == n);

    // Proprietà teorica RB: altezza <= 2 * log2(n + 1)
    std::size_t max_allowed_height = static_cast<std::size_t>(2 * std::log2(n + 1)) + 1;
    assert(tree.height() <= max_allowed_height);

    assert(tree.minimum().value() == 1);
    assert(tree.maximum().value() == n);

    // Cancellazione verificando a ogni singolo step che l'albero rimanga valido e bilanciato
    for (int i = 1; i <= n; ++i) {
        assert(tree.del(i));
        assert(tree.validate());
    }

    assert(tree.empty());
}

void test_predecessor_successor() {
    RB<int> tree;
    std::vector<int> keys = {50, 20, 70, 10, 30, 60, 80};
    for (int k : keys) tree.insert(k);

    assert(tree.validate());

    // Successor
    assert(tree.successor(10).value() == 20);
    assert(tree.successor(20).value() == 30);
    assert(tree.successor(30).value() == 50);
    assert(tree.successor(50).value() == 60);
    assert(tree.successor(80) == std::nullopt); // massimo, nessun successore

    // Predecessor
    assert(tree.predecessor(80).value() == 70);
    assert(tree.predecessor(70).value() == 60);
    assert(tree.predecessor(60).value() == 50);
    assert(tree.predecessor(50).value() == 30);
    assert(tree.predecessor(10) == std::nullopt); // minimo, nessun predecessore

    // Chiavi non presenti
    assert(tree.successor(999) == std::nullopt);
    assert(tree.predecessor(-1) == std::nullopt);
}

void test_persistence() {
    namespace fs = std::filesystem;
    fs::path temp_file = "test_rb_tree.bin";

    {
        RB<int> tree;
        std::vector<int> data = {50, 20, 80, 10, 30, 70, 90, 25};
        for (int v : data) tree.insert(v);

        tree.save(temp_file);
    } // tree distrutto fuori dallo scope

    // Ricaricamento
    RB<int> loaded = RB<int>::load(temp_file);
    assert(loaded.validate());
    assert(loaded.size() == 8);
    assert(loaded.contains(50));
    assert(loaded.contains(25));
    assert(loaded.minimum().value() == 10);
    assert(loaded.maximum().value() == 90);

    // Test persistenza albero vuoto
    {
        RB<int> empty_tree;
        empty_tree.save(temp_file);
        RB<int> empty_loaded = RB<int>::load(temp_file);
        assert(empty_loaded.empty());
        assert(empty_loaded.size() == 0);
        assert(empty_loaded.validate());
    }

    fs::remove(temp_file);
}

int main() {
    test_empty_tree();
    test_single_element();
    test_deletions_edge_cases();
    test_balance_and_rotations();
    test_predecessor_successor();
    test_persistence();

    std::cout << "Tutti i test per RB superati con successo.\n";
    return 0;
}

