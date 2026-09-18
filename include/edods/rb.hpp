#pragma once // see the eof to see an explanation

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>

namespace edods{

   /**
     * @brief Generic self-balancing Red-Black Tree storing unique keys.
     *
     * Keys are ordered using the comparison object provided through the
     * Compare template parameter. By default, std::less<Key> is used.
     *
     * The tree supports insertion, deletion, lookup, minimum, maximum,
     * predecessor, successor, validation, and snapshot persistence through
     * save() and load().
     *
     * Worst-case complexity for search, insertion, and deletion: O(log n),
     * since the tree is self-balancing through color rules and rotations.
     *
     * @tparam Key
     *     Type of the keys stored in the tree.
     *
     * @tparam Compare
     *     Callable type defining a strict weak ordering between keys.
     *     compare(a, b) must return true when a must precede b.
     */
    template< typename Key, typename Compare = std::less<Key> >
    class RB{
        public: 
            // using allow you to assign an alias to a certain type: 
            using key_type = Key; //now Key can be also used with the name key_type
            using size_type = std::size_t; // and std::size_t can be just used with size_type

            RB() = default; // it creates the default class constructor, RB(){}

            bool insert(const Key& k); //const before param means that is not possibile to modify the parameter k, but you can only read it
            bool del(const Key& k);
            bool contains(const Key& k) const; // const after parameters states that the function does not modify the object's state / class members
            
            std::optional<Key> minimum() const;
            std::optional<Key> maximum() const;
            std::optional<Key> predecessor(const Key& key) const;
            std::optional<Key> successor(const Key& key) const;

            [[nodiscard]] bool empty() const noexcept; //[[nodiscard]] is a compiler attribute for raising warnings if the programmer discards the value returned
            [[nodiscard]] size_type size() const noexcept; //noexcept is promise that the method will never raise an exceptions
            [[nodiscard]] size_type height() const noexcept;
            [[nodiscard]] bool validate() const;

            void clear() noexcept;
            void save(const std::filesystem::path& path) const;
            static RB load(const std::filesystem::path& path); //static method belongs to the class itself rather than an instance, meaning it can be called without instantiating the class.
            
        private:
            enum class Color { RED, BLACK };

            struct Node {
                explicit Node(const Key& value, Node* parent_node = nullptr, Color node_color = Color::RED) 
                    : key(value), color(node_color), parent(parent_node) {} // struct constructor. 

                Key key;
                Color color = Color::RED;
                std::unique_ptr<Node> left;  // std::unique_ptr<T> exclusively owns a dynamically allocated object and automatically destroys it when the pointer goes out of scope.
                std::unique_ptr<Node> right;
                Node* parent = nullptr;
            };
            using node = Node*;
            
            // Helper methods for red-black balance and rotations
            void rotate_left(node x);
            void rotate_right(node y);
            void insert_fixup(node z);
            void delete_fixup(node x, node x_parent, bool was_left);
            std::unique_ptr<Node>& get_owning_ptr(node n);

            bool delete_node(node n);
            void recompute_deepest(node n, size_type height);
            bool is_node_valid(node n, const Key* min_key, const Key* max_key) const;
            bool check_rb_properties(node n, int current_black_count, int& expected_black_height) const;
            void save_preorder(node n, std::ofstream& out) const;

            std::unique_ptr<Node> root_;
            size_type size_ = 0;
            size_type height_ = 0; 
            Node* deepest_node_ = nullptr;
            Compare compare_;
    
    };

}

#include "detail/rb.tpp" //in rb.tpp we will write the implementation of the prototypes defined above

/* Here are some C++ and Red-Black tree important concepts for understanding this header: 
==========================================================
 * --------------- RED-BLACK TREE PROPERTIES -------------
==========================================================
 * A Red-Black tree is a self-balancing binary search tree.
 * Every node has a color (RED or BLACK) and satisfies 5 core rules:
 *
 *  1. Every node is either RED or BLACK.
 *  2. The root is always BLACK.
 *  3. Every leaf (nullptr / NIL) is considered BLACK.
 *  4. If a node is RED, then both of its children must be BLACK.
 *     (Meaning: you can never have two RED nodes in a row on any path!).
 *  5. For each node, all simple paths from the node to descendant leaves
 *     contain the exact same number of BLACK nodes (same black-height).
 *
 * Thanks to these rules, the longest possible path (alternating red and black)
 * is at most twice as long as the shortest possible path (all black).
 * Therefore, tree height is guaranteed to be <= 2 * log2(n + 1), which means
 * search, insertion, and deletion always run in O(log n) worst-case time!
 *
==========================================================
 * ------------------- TREE ROTATIONS --------------------
==========================================================
 * Rotations are local operations on a binary tree that change the topology
 * without altering the in-order traversal (BST order remains intact!).
 *
 * Left Rotation around x:
 *        x                  y
 *       / \                / \
 *      a   y    ===>      x   c
 *         / \            / \
 *        b   c          a   b
 *
 * Right Rotation around y:
 *        y                  x
 *       / \                / \
 *      x   c    ===>      a   y
 *     / \                    / \
 *    a   b                  b   c
 *
==========================================================
 * -------------------- ENUM CLASS -----------------------
==========================================================
 * An enum class (scoped enumeration) is type-safe compared to plain enum:
 *
 *   enum class Color { RED, BLACK };
 *
 * With plain enum, RED and BLACK would leak into the surrounding scope and
 * could implicitly convert to integers.
 * With enum class, you must access them with Color::RED or Color::BLACK,
 * preventing accidental name collisions or unwanted type conversions.
 *
==========================================================
 * --------------- PRAGMA ONCE ---------------------
==========================================================
 * It is used to avoid re-declarations during compilation phase.
 * It ensures the header file is only included once per translation unit.
 *
==========================================================
* ------------------- GENERIC TYPES ----------------------
==========================================================
template< typename Key, typename Compare = std::less<Key> >
 * this tells the compiler that the class declared after depends on these 2 generic types.
 * Key and Compare are placeholders. typename is the same as "class" keyword here,
 * used for clarity inside templates.
 *
==========================================================
* --------------------- OPTIONALS ------------------------
==========================================================
* std::optional<T> is a wrapper type used when a value may or may not be present.
* It can contain either a value of type T or std::nullopt (empty state).
*
==========================================================
* --------------------- EXPLICIT ------------------------
==========================================================
* explicit prevents unwanted implicit conversions when single-argument constructors exist.
*
==========================================================
* --------------------- NAMESPACE ------------------------
==========================================================
* in C++ a namespace is a logical container used to prevent naming conflicts.
*/

