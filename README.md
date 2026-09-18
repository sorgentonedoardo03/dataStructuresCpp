# edods - C++ Data Structures Library

Welcome to **edods**! 

This repository is a lightweight, header-only C++20 data structures library written from scratch. But more than just another data structures implementation, this project is built as an **educational playground**: a place to learn how to design, organize, and build a clean modern C++ library from the ground up.

Every single header (`.hpp`) and implementation file (`.tpp`) is thoroughly commented. Instead of just showing code, the comments explain the **why** behind architectural and language choices: how smart pointers work, why ownership matters, how to avoid memory leaks with RAII, how templates behave under the hood, and how to configure modern CMake properly.

---

## Quickstart: How to Build and Run Tests

You don't need complicated setup steps. If you have CMake and a C++20 compliant compiler (like `clang++` or `g++`), all you have to do is run `make` directly from the root directory:

```bash
make
```

### What does `make` do?
Behind the scenes, the root `Makefile` does three things automatically:
1. Configures CMake build files in the `build/` directory (`cmake -B build`).
2. Compiles all test executables (`cd build && make`).
3. Executes each test suite one by one:
   - `./build/tests/bst`
   - `./build/tests/hashmap`
   - `./build/tests/rb`
   - `./build/tests/queue`

If everything is working properly, you'll see all tests pass with green output.

To clean up all build artifacts:
```bash
make clean
```

*(Note: If you want to understand how Make itself works under the hood—how it uses timestamps and a DAG to recompile only modified files—check out the commented [MakefileExample](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/MakefileExample) file in the root directory!)*

---

## Data Structures in the Library

All data structures live under the `edods::` namespace and are completely template-based and header-only.

### 1. Binary Search Tree (`edods::BST<Key, Compare>`)
* **Headers**: [`include/edods/bst.hpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/bst.hpp) & [`include/edods/detail/bst.tpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/detail/bst.tpp)
* Generic unbalanced binary search tree storing unique keys.
* Supports custom comparator types (defaults to `std::less<Key>`).
* Provides lookup (`contains`), insertion (`insert`), deletion (`del`), extremes (`minimum`, `maximum`), tree traversal navigation (`predecessor`, `successor`), and tree integrity checks (`validate`).
* **Binary Persistence**: Supports `save()` and `load()`. It serializes keys using a **pre-order traversal**, which guarantees that upon deserialization, nodes are re-inserted in the exact order required to recreate the original tree topology.

### 2. Red-Black Tree (`edods::RB<Key, Compare>`)
* **Headers**: [`include/edods/rb.hpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/rb.hpp) & [`include/edods/detail/rb.tpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/detail/rb.tpp)
* Self-balancing binary search tree that guarantees $O(\log n)$ worst-case time complexity for search, insertion, and deletion.
* Enforces Red-Black invariants (root is black, no consecutive red nodes, equal black-height along all paths).
* Implements tree rotations (`rotate_left`, `rotate_right`) and structural fixup routines after insertions and deletions.

### 3. Hash Map (`edods::HashMap<Value>`)
* **Headers**: [`include/edods/hashmap.hpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/hashmap.hpp) & [`include/edods/detail/hashmap.tpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/detail/hashmap.tpp)
* Associative key-value map with `std::string` keys and generic values.
* Uses the classic **djb2** hash algorithm by Dan Bernstein.
* **Collision Handling**: Separate chaining using a singly-linked list for each bucket. Collisions are prepended at the head of the bucket list in $O(1)$ time.
* **Dynamic Rehashing**: Monitors the load factor ($\alpha = \frac{\text{size}}{\text{bucket\_count}}$). As soon as the load factor exceeds `0.75`, it automatically doubles the bucket count and redistributes existing nodes across the new buckets without reallocating values.
* **Binary Persistence**: Full binary `save()` and `load()` serialization support for persistent storage.

### 4. Queue (`edods::Queue<T>`)
* **Headers**: [`include/edods/queue.hpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/queue.hpp) & [`include/edods/detail/queue.tpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/detail/queue.tpp)
* Generic FIFO (First-In, First-Out) queue.
* Demonstrates clean ownership design: `head_` owns the first node via `std::unique_ptr<Node>`, each node owns the next node, and `tail_` is a non-owning raw pointer (`Node*`) tracking the back of the queue. This allows $O(1)$ `push()` and `pop()` operations without circular reference or double-free issues.
* Supports binary file serialization with `save()` and `load()`.

---

## C++ Concepts Explained in This Repo

If you open the header files (especially [`bst.hpp`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/include/edods/bst.hpp)), you'll find thorough mini-tutorials explaining fundamental C++ concepts:

* **Header Guards & `#pragma once`**: Why `#pragma once` is used in modern compilers instead of old-school `#ifndef` macro guards to prevent multiple inclusion errors.
* **Header / Implementation Separation with `.tpp`**: Why template implementations can't easily be placed in separate `.cpp` files without explicit instantiation, and why including `.tpp` at the bottom of the header solves this cleanly.
* **Memory Ownership & RAII**:
  * Using `std::unique_ptr<Node>` for strict exclusive ownership.
  * Why recursive destruction (cascading `unique_ptr` reset) safely cleans up all allocated memory without manual `delete`.
  * Using non-owning raw pointers (`Node*`) for observers (like `parent` in trees or `tail_` in Queue) to avoid circular ownership leaks.
* **Templates & Dependent Qualified Names**: Why you need the `typename` keyword before dependent types (e.g. `typename BST<Key, Compare>::size_type`) so the compiler knows it's a type and not a static member variable.
* **`std::optional<T>`**: How to safely return values that might not exist (like looking for `minimum()` in an empty tree) without returning null pointers or magic error values.
* **Keywords & Compiler Attributes**:
  * `explicit`: Preventing unwanted implicit conversions in single-argument constructors.
  * `[[nodiscard]]`: Asking the compiler to warn callers if they ignore return values.
  * `noexcept`: Guaranteeing to the compiler that an operation won't throw exceptions.
  * `const` correctness: Distinguishing between const references, const methods, and pointers to const.
* **Modern CMake (`INTERFACE` libraries)**:
  * In [`CMakeLists.txt`](file:///Users/edo/Desktop/ALL/CODE/PROJECTS/C/dataStruct/CMakeLists.txt), we define `edods` as an `add_library(edods INTERFACE)` target.
  * Since this is a header-only library, it doesn't have a compilation step of its own. Using `INTERFACE` usage requirements propagates the include directory and C++20 requirement to any consumer target automatically.

---

## Repository Structure

```text
.
├── CMakeLists.txt          # Root CMake configuration (declares 'edods' INTERFACE library)
├── Makefile               # Convenient automation: builds CMake targets and runs all tests
├── MakefileExample        # Educational Makefile explaining how Make, DAGs, and rules work
├── include/
│   └── edods/             # Public headers
│       ├── bst.hpp        # Binary Search Tree (with in-depth C++ notes)
│       ├── rb.hpp         # Red-Black Tree
│       ├── hashmap.hpp    # Hash Map
│       ├── queue.hpp      # Queue
│       └── detail/        # Template implementations
│           ├── bst.tpp
│           ├── rb.tpp
│           ├── hashmap.tpp
│           └── queue.tpp
├── tests/                 # Test suites for each structure
│   ├── CMakeLists.txt
│   ├── bst.cpp
│   ├── rb.cpp
│   ├── hashmap.cpp
│   └── queue.cpp
└── examples/              # Example usage
    ├── CMakeLists.txt
    ├── main.cpp
    └── persistent_bst.cpp
```

---

## License & Usage

This project is written for educational purposes to demonstrate modern C++ design, data structure internals, and build tooling. Feel free to explore the code, read the comments, and use it as a reference for your own projects!
