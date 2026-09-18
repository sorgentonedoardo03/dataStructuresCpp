#include <iostream>
#include <string>
#include <filesystem>
#include "../include/edods/bst.hpp"
#include "../include/edods/rb.hpp"
#include "../include/edods/hashmap.hpp"
#include "../include/edods/queue.hpp"

// Example 1: Binary Search Tree (BST)
void example_bst() {
    std::cout << "========================================\n";
    std::cout << "  1. BINARY SEARCH TREE (BST) EXAMPLE   \n";
    std::cout << "========================================\n";

    edods::BST<int> tree;

    std::cout << "[BST] Inserting keys: 50, 30, 70, 20, 40, 60, 80\n";
    for (int key : {50, 30, 70, 20, 40, 60, 80}) {
        tree.insert(key);
    }

    std::cout << "[BST] Tree size: " << tree.size() << "\n";
    std::cout << "[BST] Tree height: " << tree.height() << "\n";

    // Lookup
    std::cout << "[BST] Contains 40? " << (tree.contains(40) ? "Yes" : "No") << "\n";
    std::cout << "[BST] Contains 99? " << (tree.contains(99) ? "Yes" : "No") << "\n";

    // Minimum and Maximum return std::optional
    if (auto min_val = tree.minimum()) {
        std::cout << "[BST] Minimum key: " << *min_val << "\n";
    }
    if (auto max_val = tree.maximum()) {
        std::cout << "[BST] Maximum key: " << *max_val << "\n";
    }

    // Successor and Predecessor
    if (auto succ = tree.successor(40)) {
        std::cout << "[BST] Successor of 40: " << *succ << "\n";
    }
    if (auto pred = tree.predecessor(40)) {
        std::cout << "[BST] Predecessor of 40: " << *pred << "\n";
    }

    // Persistence: save to disk and restore
    std::filesystem::path snapshot_path = "bst_example_dump.bin";
    std::cout << "[BST] Saving tree snapshot to: " << snapshot_path.string() << "\n";
    tree.save(snapshot_path);

    std::cout << "[BST] Loading tree snapshot from file...\n";
    edods::BST<int> restored_tree = edods::BST<int>::load(snapshot_path);
    std::cout << "[BST] Restored tree size: " << restored_tree.size() 
              << ", valid: " << (restored_tree.validate() ? "true" : "false") << "\n";

    std::filesystem::remove(snapshot_path);
    std::cout << "\n";
}

// Example 2: Red-Black Tree (Self-Balancing)
void example_rb() {
    std::cout << "========================================\n";
    std::cout << "  2. RED-BLACK (RB) TREE EXAMPLE        \n";
    std::cout << "========================================\n";

    edods::RB<int> rb_tree;

    // In an unbalanced BST, inserting strictly ascending numbers creates a degenerate list of height 7.
    // In a Red-Black tree, rotations and recoloring maintain O(log n) balance!
    std::cout << "[RB] Inserting ordered keys: 10, 20, 30, 40, 50, 60, 70\n";
    for (int key : {10, 20, 30, 40, 50, 60, 70}) {
        rb_tree.insert(key);
    }

    std::cout << "[RB] Size: " << rb_tree.size() << "\n";
    std::cout << "[RB] Height: " << rb_tree.height() << "\n";
    std::cout << "[RB] Contains 30? " << (rb_tree.contains(30) ? "Yes" : "No") << "\n";

    std::cout << "[RB] Deleting key 20...\n";
    rb_tree.del(20);
    std::cout << "[RB] Contains 20 after deletion? " << (rb_tree.contains(20) ? "Yes" : "No") << "\n";
    std::cout << "[RB] Size after deletion: " << rb_tree.size() << "\n\n";
}

// Example 3: HashMap with Automatic Rehashing
void example_hashmap() {
    std::cout << "========================================\n";
    std::cout << "  3. HASHMAP (SEPARATE CHAINING) EXAMPLE \n";
    std::cout << "========================================\n";

    // Initial bucket capacity set to 8 for demonstrating rehash
    edods::HashMap<double> student_grades(8);

    std::cout << "[HashMap] Initial bucket count: " << student_grades.bucket_count() << "\n";

    // Insert key-value pairs
    student_grades.insert("Mario", 28.5);
    student_grades.insert("Luigi", 30.0);
    student_grades.insert("Peach", 27.0);
    student_grades.insert("Toad", 24.0);
    student_grades.insert("Yoshi", 29.0);
    student_grades.insert("Bowser", 18.0); // 6th element: load factor is 6/8 = 0.75

    std::cout << "[HashMap] Inserted 6 elements. Size = " << student_grades.size() 
              << ", Bucket count = " << student_grades.bucket_count() 
              << ", Load factor = " << student_grades.load_perc() << "\n";

    // At 6 elements, load factor is 6/8 = 0.75.
    // Inserting "Wario" makes size = 7 (lf = 7/8 = 0.875).
    // The next insertion ("Daisy") detects lf > 0.75 and triggers an automatic rehash (doubling buckets to 16)!
    std::cout << "[HashMap] Inserting 'Wario' (size becomes 7, lf = 0.875)...\n";
    student_grades.insert("Wario", 22.0);
    std::cout << "[HashMap] Inserting 'Daisy' to trigger automatic rehash...\n";
    student_grades.insert("Daisy", 26.5);

    std::cout << "[HashMap] After rehash -> Size: " << student_grades.size() 
              << ", Bucket count: " << student_grades.bucket_count()
              << ", Load factor: " << student_grades.load_perc() << "\n";

    // Lookups (returns std::optional<Value>)
    if (auto grade = student_grades.get("Luigi")) {
        std::cout << "[HashMap] Grade for 'Luigi': " << *grade << "\n";
    }

    // Removing a key
    std::cout << "[HashMap] Removing 'Bowser'...\n";
    student_grades.del("Bowser");
    std::cout << "[HashMap] Key 'Bowser' present after deletion? " 
              << (student_grades.get("Bowser").has_value() ? "Yes" : "No") << "\n\n";
}

// Example 4: Queue (FIFO with clean modern ownership)
void example_queue() {
    std::cout << "========================================\n";
    std::cout << "  4. QUEUE (FIFO) EXAMPLE               \n";
    std::cout << "========================================\n";

    edods::Queue<std::string> print_jobs;

    std::cout << "[Queue] Enqueuing print jobs...\n";
    if (print_jobs.enqueue("Document_1.pdf")) {
        std::cout << "  -> Queued Document_1.pdf\n";
    }
    if (print_jobs.enqueue("Report_Final.docx")) {
        std::cout << "  -> Queued Report_Final.docx\n";
    }
    if (print_jobs.enqueue("Invoice_2026.pdf")) {
        std::cout << "  -> Queued Invoice_2026.pdf\n";
    }

    std::cout << "[Queue] Current queue size: " << print_jobs.size() << "\n";

    std::cout << "[Queue] Processing and dequeuing jobs (FIFO order):\n";
    while (print_jobs.size() > 0) {
        auto job = print_jobs.dequeue();
        if (job) {
            std::cout << "  -> Processed: " << *job << "\n";
        }
    }

    std::cout << "[Queue] Queue size after processing: " << print_jobs.size() << "\n";
    std::cout << "========================================\n";
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "       edods LIBRARY - USAGE EXAMPLES             \n";
    std::cout << "==================================================\n\n";

    example_bst();
    example_rb();
    example_hashmap();
    example_queue();

    std::cout << "All examples completed successfully!\n";
    return 0;
}