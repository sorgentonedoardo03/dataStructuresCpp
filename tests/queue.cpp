#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>
#include "../include/edods/queue.hpp"

using namespace edods;

void test_empty_queue() {
    Queue<int> q;
    assert(q.size() == 0);
    assert(!q.dequeue().has_value());

    // Test clear su vuoto
    q.clear();
    assert(q.size() == 0);
    assert(!q.dequeue().has_value());
}

void test_enqueue_dequeue() {
    Queue<int> q;
    assert(q.enqueue(10));
    assert(q.size() == 1);

    assert(q.enqueue(20));
    assert(q.enqueue(30));
    assert(q.size() == 3);

    // Verifica dell'ordine FIFO (First-In, First-Out)
    auto v1 = q.dequeue();
    assert(v1.has_value());
    assert(v1.value() == 10);
    assert(q.size() == 2);

    auto v2 = q.dequeue();
    assert(v2.has_value());
    assert(v2.value() == 20);
    assert(q.size() == 1);

    auto v3 = q.dequeue();
    assert(v3.has_value());
    assert(v3.value() == 30);
    assert(q.size() == 0);

    // Coda ora vuota
    assert(!q.dequeue().has_value());
}

void test_interleaved_operations() {
    Queue<int> q;
    // Inserimenti e rimozioni alternate
    assert(q.enqueue(1));
    assert(q.enqueue(2));
    assert(q.dequeue().value() == 1);

    assert(q.enqueue(3));
    assert(q.enqueue(4));
    assert(q.dequeue().value() == 2);
    assert(q.dequeue().value() == 3);

    assert(q.enqueue(5));
    assert(q.dequeue().value() == 4);
    assert(q.dequeue().value() == 5);
    assert(q.size() == 0);
    assert(!q.dequeue().has_value());
}

void test_clear() {
    Queue<int> q;
    for (int i = 1; i <= 5; ++i) {
        assert(q.enqueue(i * 10));
    }
    assert(q.size() == 5);

    q.clear();
    assert(q.size() == 0);
    assert(!q.dequeue().has_value());

    // Reinserimento dopo clear
    assert(q.enqueue(99));
    assert(q.size() == 1);
    assert(q.dequeue().value() == 99);
    assert(q.size() == 0);
}

void test_persistence() {
    namespace fs = std::filesystem;
    fs::path temp_file = "test_queue.bin";

    {
        Queue<int> q;
        for (int i = 1; i <= 10; ++i) {
            assert(q.enqueue(i * 100));
        }
        assert(q.size() == 10);
        q.save(temp_file);
    } // q distrutta fuori dallo scope

    // Ricaricamento da file
    Queue<int> loaded = Queue<int>::load(temp_file);
    assert(loaded.size() == 10);

    // Verifica che l'ordine FIFO sia preservato dopo il load
    for (int i = 1; i <= 10; ++i) {
        auto val = loaded.dequeue();
        assert(val.has_value());
        assert(val.value() == i * 100);
    }
    assert(loaded.size() == 0);

    // Test persistenza coda vuota
    {
        Queue<int> empty_q;
        empty_q.save(temp_file);
        Queue<int> empty_loaded = Queue<int>::load(temp_file);
        assert(empty_loaded.size() == 0);
        assert(!empty_loaded.dequeue().has_value());
    }

    fs::remove(temp_file);
}

int main() {
    test_empty_queue();
    test_enqueue_dequeue();
    test_interleaved_operations();
    test_clear();
    test_persistence();

    std::cout << "Tutti i test superati con successo.\n";
    return 0;
}

