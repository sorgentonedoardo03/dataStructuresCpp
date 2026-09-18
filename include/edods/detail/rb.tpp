#include <optional>
#include <fstream>
#include <stdexcept>

namespace edods {

    /**
     * @brief Checks whether the tree is empty.
     *
     * @return true if the tree contains no nodes, false otherwise.
     *
     * @post The state of the tree remains unchanged.
     *
     * @note Time complexity: O(1).
     * @note Space complexity: O(1).
     */
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::empty() const noexcept {
        return this->size_ == 0; 
    } 

    /**
     * @brief Returns the number of keys stored in the tree.
     *
     * This operation does not modify the tree.
     *
     * @return The current number of nodes in the tree.
     *
     * @post The state of the tree remains unchanged.
     *
     * @note Time complexity: O(1).
     * @note Space complexity: O(1).
     */
    template<typename Key, typename Compare>
    //the function return type is size_type
    typename RB<Key, Compare>::size_type RB<Key, Compare>::size() const noexcept {
        return size_; // this-> is implicit
    }

    /**
     * @brief Returns the height of the Red-Black tree
     * 
     * This operation does not modify the tree.
     * @return the number of nodes of the highest branch in the tree
     * @post the state of the tree remains unchanged
     * @note Time Complexity: O(1)
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    typename RB<Key, Compare>::size_type RB<Key, Compare>::height() const noexcept {
        return height_;
    }

    /**
     * @brief Removes all elements from the tree, leaving it empty.
     * Destroys all nodes in the tree and resets size, height, and node pointers.
     * Memory is deterministically deallocated via RAII through unique_ptr cascading.
     * @post empty() == true
     * @post size() == 0
     * @post height() == 0
     * @note Time Complexity: O(n), where n is the number of nodes in the tree
     * @note Space Complexity: O(log n) call stack frames due to destruction in a balanced tree
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::clear() noexcept {
        root_.reset();
        size_ = 0;
        height_ = 0;
        deepest_node_ = nullptr;
    }

    /**
     * @brief Returns the minimum key stored in the tree
     * @post the state of the tree remain unchanged
     * @return the minimum of the RB tree
     * @note Time Complexity: O(log n)
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    std::optional<Key> RB<Key, Compare>::minimum() const {
        if (root_ == nullptr)
            return std::nullopt;

        node curr = root_.get();

        while (curr->left != nullptr)
            curr = curr->left.get();

        return curr->key;
    }

    /**
     * @brief Returns the maximum key stored in the tree
     * @post the state of the tree remain unchanged
     * @return the maximum of the RB tree
     * @note Time Complexity: O(log n)
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    std::optional<Key> RB<Key, Compare>::maximum() const {
        if (root_ == nullptr)
            return std::nullopt;

        node curr = root_.get();

        while (curr->right != nullptr)
            curr = curr->right.get();

        return curr->key;
    }

    /**
     * @brief Returns true if the key is in the tree by performing binary search
     * @post the state of the tree remain unchanged
     * @return true if the key is in the tree, false otherwise
     * @note Time Complexity: O(log n)
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::contains(const Key& k) const {
        node curr = root_.get(); // get() returns the raw Node* stored inside root_ without transferring ownership(unique_ptr)
        while (curr != nullptr) {
            if (!compare_(k, curr->key) && !compare_(curr->key, k))
                return true;
            if (compare_(k, curr->key))
                curr = curr->left.get();
            else
                curr = curr->right.get();
        }
        return false;
    }

    /**
     * @brief Returns the successor of the specified key in the RB tree.
     *
     * The successor is the smallest key strictly greater than the given key.
     *
     * @param key The key whose successor has to be found.
     * @return The successor key if it exists, std::nullopt otherwise.
     *
     * @post The state of the tree remains unchanged.
     *
     * @note Time complexity: O(log n).
     * @note Space complexity: O(1).
     */
    template<typename Key, typename Compare>
    std::optional<Key> RB<Key, Compare>::successor(const Key& key) const {
        // getting the node from the key
        node curr = root_.get(); // get() returns the raw Node* stored inside root_ without transferring ownership(unique_ptr)
        while (curr != nullptr) {
            if (!compare_(key, curr->key) && !compare_(curr->key, key))
                break;
            if (compare_(key, curr->key))
                curr = curr->left.get();
            else
                curr = curr->right.get();
        }
        if (curr == nullptr) {
            return std::nullopt;
        }

        // If the right subtree exists the successor is its minimum node.
        if (curr->right != nullptr) {
            curr = curr->right.get();

            while (curr->left != nullptr)
                curr = curr->left.get();

            return curr->key;
        }

        // Otherwise, go upward until we find an ancestor for which curr belongs to the left subtree.
        node parent = curr->parent;
        while (parent != nullptr && curr == parent->right.get()) {
            curr = parent;
            parent = parent->parent;
        }

        // No successor exists if parent == nullptr.
        if (parent == nullptr)
            return std::nullopt;

        return parent->key;
    }

    /**
     * @brief predecessor of a key in the tree
     * @param key it is the key of the node whose predecessor you want to find
     * @return the predecessor of key
     */
    template<typename Key, typename Compare>
    std::optional<Key> RB<Key, Compare>::predecessor(const Key& key) const {
        // let's find the node: 
        node temp = root_.get();
        while (temp != nullptr) {
            if (!compare_(temp->key, key) && !compare_(key, temp->key)) {
                break;
            }
            if (compare_(key, temp->key))
                temp = temp->left.get();
            else
                temp = temp->right.get();
        }
        if (temp == nullptr)
            return std::nullopt;
        
        // CASE 1: if the left subtree exists, the predecessor is the maximum node of the left subtree.
        if (temp->left != nullptr) {
            node curr = temp->left.get();

            while (curr->right != nullptr)
                curr = curr->right.get();

            return curr->key;
        }

        // CASE 2: No left subtree: move upward through the parents.
        node curr = temp;
        node parent = temp->parent;

        // While curr is the left child of its parent, that parent cannot be the predecessor.
        while (parent != nullptr && curr == parent->left.get()) {
            curr = parent;
            parent = parent->parent;
        }

        // If parent is nullptr, no predecessor exists.
        if (parent == nullptr)
            return std::nullopt;

        return parent->key;
    }

    // AUX helper to get the unique_ptr reference that owns node n
    template<typename Key, typename Compare>
    std::unique_ptr<typename RB<Key, Compare>::Node>& RB<Key, Compare>::get_owning_ptr(node n) {
        if (n->parent == nullptr) {
            return root_;
        }
        if (n->parent->left.get() == n) {
            return n->parent->left;
        }
        return n->parent->right;
    }

    /**
     * @brief Left rotation around node x
     * 
     * Moves x down to the left and brings x's right child y up to take x's place.
     * Preserves binary search tree order.
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::rotate_left(node x) {
        if (x == nullptr || x->right == nullptr)
            return;

        node y = x->right.get();
        node p = x->parent;

        // Get reference to the unique_ptr holding x (could be root_ or a parent's child pointer)
        std::unique_ptr<Node>& own_x = get_owning_ptr(x);

        // 1. Extract y out of x->right
        std::unique_ptr<Node> y_ptr = std::move(x->right);

        // 2. y's left subtree beta becomes x's right subtree
        x->right = std::move(y->left);
        if (x->right != nullptr) {
            x->right->parent = x;
        }

        // 3. Update parents
        y->parent = p;
        x->parent = y;

        // 4. x becomes left child of y
        y->left = std::move(own_x);

        // 5. y takes the position where x was originally held
        own_x = std::move(y_ptr);
    }

    /**
     * @brief Right rotation around node y
     * 
     * Moves y down to the right and brings y's left child x up to take y's place.
     * Preserves binary search tree order.
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::rotate_right(node y) {
        if (y == nullptr || y->left == nullptr)
            return;

        node x = y->left.get();
        node p = y->parent;

        // Get reference to the unique_ptr holding y
        std::unique_ptr<Node>& own_y = get_owning_ptr(y);

        // 1. Extract x out of y->left
        std::unique_ptr<Node> x_ptr = std::move(y->left);

        // 2. x's right subtree beta becomes y's left subtree
        y->left = std::move(x->right);
        if (y->left != nullptr) {
            y->left->parent = y;
        }

        // 3. Update parents
        x->parent = p;
        y->parent = x;

        // 4. y becomes right child of x
        x->right = std::move(own_y);

        // 5. x takes the position where y was originally held
        own_y = std::move(x_ptr);
    }

    /**
     * @brief Restores Red-Black Tree properties after a new red node insertion.
     * 
     * Fixes any red-red violations between z and z->parent through recoloring
     * and rotations.
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::insert_fixup(node z) {
        while (z != nullptr && z->parent != nullptr && z->parent->color == Color::RED) {
            node p = z->parent;
            node g = p->parent;

            if (g == nullptr)
                break;

            // CASE A: parent p is left child of grandparent g
            if (p == g->left.get()) {
                node uncle = g->right.get();

                // Case 1: uncle is RED -> push black down from grandparent
                if (uncle != nullptr && uncle->color == Color::RED) {
                    p->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    g->color = Color::RED;
                    z = g; // continue checking up at grandparent
                } else {
                    // Case 2: uncle is BLACK and z is right child (triangle / zig-zag)
                    if (z == p->right.get()) {
                        z = p;
                        rotate_left(z);
                        p = z->parent;
                        g = p->parent;
                    }

                    // Case 3: uncle is BLACK and z is left child (line)
                    p->color = Color::BLACK;
                    g->color = Color::RED;
                    rotate_right(g);
                }
            } else {
                // CASE B: parent p is right child of grandparent g (symmetric)
                node uncle = g->left.get();

                // Case 1: uncle is RED -> push black down
                if (uncle != nullptr && uncle->color == Color::RED) {
                    p->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    g->color = Color::RED;
                    z = g;
                } else {
                    // Case 2: uncle is BLACK and z is left child (triangle / zig-zag)
                    if (z == p->left.get()) {
                        z = p;
                        rotate_right(z);
                        p = z->parent;
                        g = p->parent;
                    }

                    // Case 3: uncle is BLACK and z is right child (line)
                    p->color = Color::BLACK;
                    g->color = Color::RED;
                    rotate_left(g);
                }
            }
        }

        // Rule 2: root must always be BLACK!
        if (root_ != nullptr) {
            root_->color = Color::BLACK;
        }
    }

    /**
     * @brief Insert a node in the tree by passing the key
     * @param key is the key to insert in the RB tree
     * @post tree.contains(key) = true, tree remains balanced, height and deepest node are updated.
     * @note Time complexity is O(log n)
     * @note Space complexity is O(1)
     */
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::insert(const Key& key) {
        node y = nullptr;
        node x = root_.get();

        // 1. finding the parent where to attach new node
        while (x != nullptr) {
            y = x;
            if (compare_(key, x->key)) {
                x = x->left.get();
            } else if (compare_(x->key, key)) {
                x = x->right.get();
            } else {
                return false; // duplicate key
            }
        }

        // 2. creating the node (new nodes in Red-Black trees are always RED initially)
        auto n = std::make_unique<Node>(key, y, Color::RED);
        node raw_n = n.get();

        // 3. inserting it in the tree
        if (y == nullptr) {
            root_ = std::move(n);
        } else {
            if (compare_(key, y->key)) {
                y->left = std::move(n);
            } else {
                y->right = std::move(n);
            }
        }

        ++size_;

        // 4. fix red-black properties after insertion
        insert_fixup(raw_n);

        // 5. height and deepest node update
        height_ = 0;
        deepest_node_ = nullptr;
        if (root_ != nullptr) {
            recompute_deepest(root_.get(), 1);
        }

        return true;
    }

    /**
     * @brief Fixes double-black violations after deleting a black node.
     * 
     * @param x The replacement node (may be nullptr).
     * @param x_parent Parent of x.
     * @param was_left True if x was the left child of x_parent.
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::delete_fixup(node x, node x_parent, bool was_left) {
        while (x != root_.get() && x_parent != nullptr && (x == nullptr || x->color == Color::BLACK)) {
            bool is_left = (x != nullptr) ? (x == x_parent->left.get()) : was_left;

            if (is_left) {
                node w = x_parent->right.get(); // sibling

                // CASE 1: sibling w is red
                if (w != nullptr && w->color == Color::RED) {
                    w->color = Color::BLACK;
                    x_parent->color = Color::RED;
                    rotate_left(x_parent);
                    w = x_parent->right.get();
                }

                // CASE 2: sibling w is black, and both of its children are black
                bool left_black = (w == nullptr || w->left == nullptr || w->left->color == Color::BLACK);
                bool right_black = (w == nullptr || w->right == nullptr || w->right->color == Color::BLACK);

                if (left_black && right_black) {
                    if (w != nullptr) {
                        w->color = Color::RED;
                    }
                    x = x_parent;
                    x_parent = x->parent;
                } else {
                    // CASE 3: sibling w is black, w->right is black, w->left is red
                    if (right_black) {
                        if (w != nullptr && w->left != nullptr) {
                            w->left->color = Color::BLACK;
                        }
                        if (w != nullptr) {
                            w->color = Color::RED;
                            rotate_right(w);
                            w = x_parent->right.get();
                        }
                    }

                    // CASE 4: sibling w is black and w->right is red
                    if (w != nullptr) {
                        w->color = x_parent->color;
                        if (w->right != nullptr) {
                            w->right->color = Color::BLACK;
                        }
                    }
                    x_parent->color = Color::BLACK;
                    rotate_left(x_parent);
                    x = root_.get();
                    break;
                }
            } else {
                // Symmetric: x is right child
                node w = x_parent->left.get(); // sibling

                // CASE 1: sibling w is red
                if (w != nullptr && w->color == Color::RED) {
                    w->color = Color::BLACK;
                    x_parent->color = Color::RED;
                    rotate_right(x_parent);
                    w = x_parent->left.get();
                }

                // CASE 2: sibling w is black, and both children are black
                bool left_black = (w == nullptr || w->left == nullptr || w->left->color == Color::BLACK);
                bool right_black = (w == nullptr || w->right == nullptr || w->right->color == Color::BLACK);

                if (left_black && right_black) {
                    if (w != nullptr) {
                        w->color = Color::RED;
                    }
                    x = x_parent;
                    x_parent = x->parent;
                } else {
                    // CASE 3: sibling w is black, w->left is black, w->right is red
                    if (left_black) {
                        if (w != nullptr && w->right != nullptr) {
                            w->right->color = Color::BLACK;
                        }
                        if (w != nullptr) {
                            w->color = Color::RED;
                            rotate_left(w);
                            w = x_parent->left.get();
                        }
                    }

                    // CASE 4: sibling w is black, w->left is red
                    if (w != nullptr) {
                        w->color = x_parent->color;
                        if (w->left != nullptr) {
                            w->left->color = Color::BLACK;
                        }
                    }
                    x_parent->color = Color::BLACK;
                    rotate_right(x_parent);
                    x = root_.get();
                    break;
                }
            }
        }

        if (x != nullptr) {
            x->color = Color::BLACK;
        }
        if (root_ != nullptr) {
            root_->color = Color::BLACK;
        }
    }

    /**
     * @brief Deletes a node from the Red-Black tree and restores balance.
     * 
     * Handles the standard cases:
     *  - node with two children: replaces its key with its successor's key and deletes the successor.
     *  - node with at most one child: unlinks the node and triggers delete_fixup if a black node was removed.
     * 
     * @param n Pointer to the node to delete.
     * @return true if the node was deleted, false if n is nullptr.
     * 
     * @note Time complexity: O(log n).
     * @note Space complexity: O(1).
     */
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::delete_node(node n) {
        if (n == nullptr)
            return false;

        // CASE 3: two children.
        // We find the successor in the right subtree, copy its key to n,
        // and delete the successor instead (which has at most one child!).
        if (n->left != nullptr && n->right != nullptr) {
            node successor = n->right.get();
            while (successor->left != nullptr) {
                successor = successor->left.get();
            }
            n->key = successor->key;
            return delete_node(successor);
        }

        // Now n has at most one child:
        node x_parent = n->parent;
        bool was_left = (x_parent != nullptr && x_parent->left.get() == n);
        Color removed_color = n->color;

        std::unique_ptr<Node> child_ptr;
        if (n->left != nullptr) {
            child_ptr = std::move(n->left);
        } else if (n->right != nullptr) {
            child_ptr = std::move(n->right);
        }

        node x = child_ptr.get();
        if (x != nullptr) {
            x->parent = x_parent;
        }

        // Detach n and splice child into parent or root_
        if (x_parent == nullptr) {
            root_ = std::move(child_ptr);
        } else if (was_left) {
            x_parent->left = std::move(child_ptr);
        } else {
            x_parent->right = std::move(child_ptr);
        }

        --size_;

        // If the removed node was BLACK, balance was disrupted.
        // If the replacement child is RED, coloring it BLACK immediately fixes it!
        if (removed_color == Color::BLACK) {
            if (x != nullptr && x->color == Color::RED) {
                x->color = Color::BLACK;
            } else {
                delete_fixup(x, x_parent, was_left);
            }
        }

        return true;
    }

    /**
     * @brief Recomputes the deepest node and the height of the RB tree.
     *
     * Performs a DFS starting from the given node, keeping track of the
     * current depth. Whenever a node deeper than the current maximum is found,
     * height_ and deepest_node_ are updated.
     *
     * @param n Current node being visited.
     * @param height Depth of the current node.
     *
     * @note Time complexity: O(n), since every node may be visited once.
     * @note Space complexity: O(log n), due to the balanced tree height.
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::recompute_deepest(node n, size_type height) {
        if (n == nullptr)
            return;

        if (height > height_) {
            height_ = height;
            deepest_node_ = n;
        }

        recompute_deepest(n->left.get(), height + 1);
        recompute_deepest(n->right.get(), height + 1);
    }

    /**
     * @brief Delete the passed key from the tree and returns true if correctly deleted 
     * @param n is the key of the node to delete
     * @return true if correctly deleted, false otherwise
     * @post deletes the node, restores RB balance and updates tree height.
     * @note Time Complexity: O(log n)
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::del(const Key& n) {
        // getting the node from the key
        node to_del = root_.get();
        while (to_del != nullptr) {
            if (!compare_(n, to_del->key) && !compare_(to_del->key, n))
                break;
            if (compare_(n, to_del->key))
                to_del = to_del->left.get();
            else
                to_del = to_del->right.get();
        }
        if (to_del == nullptr)
            return false;

        bool res = delete_node(to_del);
        if (res) {
            height_ = 0;
            deepest_node_ = nullptr;
            if (root_ != nullptr) {
                recompute_deepest(root_.get(), 1);
            }
        }
        return res;
    }

    // AUX function for validate(): checks BST ordering
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::is_node_valid(node n, const Key* min_key, const Key* max_key) const {
        if (n == nullptr)
            return true;

        if (min_key != nullptr && !compare_(*min_key, n->key))
            return false;

        if (max_key != nullptr && !compare_(n->key, *max_key))
            return false;

        return is_node_valid(n->left.get(), min_key, &(n->key)) &&
               is_node_valid(n->right.get(), &(n->key), max_key);
    }

    // AUX function for validate(): checks Red-Black properties (no red-red, equal black height, parent links)
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::check_rb_properties(node n, int current_black_count, int& expected_black_height) const {
        if (n == nullptr) {
            // Reached a leaf (null leaves count as black)
            if (expected_black_height == -1) {
                expected_black_height = current_black_count;
                return true;
            }
            return current_black_count == expected_black_height;
        }

        // Parent pointer integrity
        if (n->left != nullptr && n->left->parent != n)
            return false;
        if (n->right != nullptr && n->right->parent != n)
            return false;

        // Red property: if a node is red, both children must be black
        if (n->color == Color::RED) {
            if (n->left != nullptr && n->left->color == Color::RED)
                return false;
            if (n->right != nullptr && n->right->color == Color::RED)
                return false;
        }

        int next_black = current_black_count + (n->color == Color::BLACK ? 1 : 0);

        return check_rb_properties(n->left.get(), next_black, expected_black_height) &&
               check_rb_properties(n->right.get(), next_black, expected_black_height);
    }

    /**
     * @brief Checks whether the Red-Black Tree satisfies all binary search and RB invariants.
     * @return true if the tree is valid, false otherwise.
     * @post The state of the tree remains unchanged.
     */
    template<typename Key, typename Compare>
    bool RB<Key, Compare>::validate() const {
        if (root_ == nullptr) {
            return size_ == 0;
        }

        // Rule 2: Root must be BLACK
        if (root_->color != Color::BLACK) {
            return false;
        }

        // Check BST ordering
        if (!is_node_valid(root_.get(), nullptr, nullptr)) {
            return false;
        }

        // Check RB properties (no two adjacent reds, identical black height for all paths)
        int expected_black_height = -1;
        return check_rb_properties(root_.get(), 0, expected_black_height);
    }

    //-------------- PERSISTENCE ------------------
    // AUX method for save()
    // writes recursively nodes in the file in pre-order
    template<typename Key, typename Compare>
    void RB<Key, Compare>::save_preorder(node n, std::ofstream& out) const {
        if (n == nullptr) {
            return;
        }
        // 1. writes the root
        out.write(reinterpret_cast<const char*>(&(n->key)), sizeof(Key));
        // 2. left subtree visit
        save_preorder(n->left.get(), out);
        // 3. right subtree visit
        save_preorder(n->right.get(), out);
    }

    /**
     * @brief Serializes the RB tree to persistent storage at the specified path.
     * Uses binary pre-order traversal serialization.
     * @param path Filesystem path where the binary dump will be written.
     * @throws std::runtime_error if the destination file cannot be opened.
     * @note Time Complexity: O(n), where n is the number of nodes.
     * @note Space Complexity: O(log n) recursion depth.
     */
    template<typename Key, typename Compare>
    void RB<Key, Compare>::save(const std::filesystem::path& path) const {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) {
            throw std::runtime_error("Error opening the file: " + path.string());
        }

        // Header: writing the size of RB
        // it casts the address of size_ to a byte pointer (char*) so out.write can read and dump its exact 8 bytes of memory directly into the file
        out.write(reinterpret_cast<const char*>(&size_), sizeof(size_type));

        // writing keys in pre-order
        save_preorder(root_.get(), out);
        // Flush and verify that all bytes were written successfully to disk
        out.flush();
        if (!out.good()) {
            throw std::runtime_error("I/O error while writing data to file: " + path.string());
        }
    }

    /**
     * @brief Deserializes an RB tree from persistent storage.
     * Reads the serialized binary file, extracts the total size, and sequentially
     * inserts the keys to reconstruct the tree.
     * @param path Filesystem path of the binary dump to load.
     * @return RB<Key, Compare> A newly reconstructed RB instance.
     * @throws std::runtime_error if the file cannot be opened or if I/O read fails.
     * @note Time Complexity: O(n log n) due to sequential insertions and rebalancing.
     * @note Space Complexity: O(n) to allocate the reconstructed tree nodes.
     */
    template<typename Key, typename Compare>
    RB<Key, Compare> RB<Key, Compare>::load(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) {
            throw std::runtime_error("Error opening the file for reading: " + path.string());
        }

        // 1. Read the size saved in the header
        size_type saved_size = 0;
        in.read(reinterpret_cast<char*>(&saved_size), sizeof(size_type));
        if (!in.good()) {
            throw std::runtime_error("Error reading header from file: " + path.string());
        }

        RB<Key, Compare> tree;

        // 2. Read each key and insert them one by one
        for (size_type i = 0; i < saved_size; ++i) {
            Key k{};
            in.read(reinterpret_cast<char*>(&k), sizeof(Key));
            
            if (!in.good()) {
                throw std::runtime_error("Corrupted file or unexpected EOF while reading keys: " + path.string());
            }

            tree.insert(k);
        }

        return tree;
    }

}

