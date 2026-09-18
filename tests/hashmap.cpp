#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>
#include "../include/edods/hashmap.hpp"

using namespace edods;

void test_empty_map() {
    HashMap<int> map;
    assert(map.empty());
    assert(map.size() == 0);
    assert(map.bucket_count() == 100);
    assert(map.load_perc() == 0.0f);
    assert(!map.get("ghost").has_value());
    assert(!map.del("ghost"));

    // Test clear su vuoto
    map.clear();
    assert(map.empty());
    assert(map.size() == 0);
}

void test_insert_and_get() {
    HashMap<int> map;
    assert(map.insert("alice", 100));
    assert(!map.empty());
    assert(map.size() == 1);
    assert(map.get("alice").has_value());
    assert(map.get("alice").value() == 100);

    // Reinserimento duplicato
    assert(!map.insert("alice", 999));
    assert(map.size() == 1);
    assert(map.get("alice").value() == 100);

    // Altri inserimenti
    assert(map.insert("bob", 200));
    assert(map.insert("charlie", 300));
    assert(map.size() == 3);
    assert(map.get("bob").value() == 200);
    assert(map.get("charlie").value() == 300);
}

void test_deletion() {
    HashMap<int> map;
    map.insert("k1", 10);
    map.insert("k2", 20);
    map.insert("k3", 30);
    assert(map.size() == 3);

    // Cancellazione di chiave non esistente
    assert(!map.del("k4"));
    assert(map.size() == 3);

    // Cancellazione corretta
    assert(map.del("k2"));
    assert(map.size() == 2);
    assert(!map.get("k2").has_value());
    assert(map.get("k1").value() == 10);
    assert(map.get("k3").value() == 30);

    // Re-eliminazione della stessa chiave
    assert(!map.del("k2"));

    // Eliminazione delle rimanenti
    assert(map.del("k1"));
    assert(map.del("k3"));
    assert(map.empty());
    assert(map.size() == 0);
}

void test_rehash_and_scale() {
    // Inizializziamo con 10 bucket per forzare il rehash prima
    HashMap<int> map(10);
    assert(map.bucket_count() == 10);

    // Inseriamo elementi fino a superare la soglia di carico (10 * 0.75 = 7.5)
    for (int i = 0; i < 20; ++i) {
        std::string key = "key_" + std::to_string(i);
        assert(map.insert(key, i * 10));
    }

    assert(map.size() == 20);
    // La tabella deve essersi espansa
    assert(map.bucket_count() > 10);

    // Verifichiamo che tutti gli elementi siano ancora recuperabili dopo il rehash
    for (int i = 0; i < 20; ++i) {
        std::string key = "key_" + std::to_string(i);
        auto val = map.get(key);
        assert(val.has_value());
        assert(val.value() == i * 10);
    }
}

void test_persistence() {
    namespace fs = std::filesystem;
    fs::path temp_file = "test_hashmap.bin";

    {
        HashMap<int> map;
        for (int i = 0; i < 50; ++i) {
            map.insert("user_" + std::to_string(i), i * 100);
        }
        assert(map.size() == 50);
        map.save(temp_file);
    } // map distrutta fuori dallo scope

    // Ricaricamento
    HashMap<int> loaded = HashMap<int>::load(temp_file);
    assert(loaded.size() == 50);

    for (int i = 0; i < 50; ++i) {
        auto val = loaded.get("user_" + std::to_string(i));
        assert(val.has_value());
        assert(val.value() == i * 100);
    }

    // Test persistenza mappa vuota
    {
        HashMap<int> empty_map;
        empty_map.save(temp_file);
        HashMap<int> empty_loaded = HashMap<int>::load(temp_file);
        assert(empty_loaded.empty());
        assert(empty_loaded.size() == 0);
    }

    fs::remove(temp_file);
}

int main() {
    test_empty_map();
    test_insert_and_get();
    test_deletion();
    test_rehash_and_scale();
    test_persistence();

    std::cout << "Tutti i test superati con successo.\n";
    return 0;
}

