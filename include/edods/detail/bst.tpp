#include <optional>
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
    bool BST<Key, Compare>::empty() const noexcept {
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
    //the function return type is  size_type
    typename BST<Key, Compare>::size_type  BST<Key, Compare>::size() const noexcept {
        return size_; // this-> is implicit
    }


    /**
     * @brief Returns the height of the BST
     * 
     * This operation does not modifies the tree.
     * @return the number of nodes of the highest branch in the tree
     * @post the state of the tree remains unchanged
     * @note Time Complexity: O(1)
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    typename BST<Key,Compare>::size_type BST<Key,Compare>::height() const noexcept {
        return height_;
    }

    /**
     * @brief Returns the minimum key stored in the tree
     * @post the state of the tree remain unchanged
     * @return the minimum of the BST
     * @note Time Complexity: O(h) where h is the tree height
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    std::optional<Key> BST<Key, Compare>::minimum() const {
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
     * @return the maximum of the BST
     * @note Time Complexity: O(h) where h is the tree height
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    std::optional<Key> BST<Key, Compare>::maximum() const {

        if (root_ == nullptr)
            return std::nullopt;

        node curr = root_.get();

        while (curr->right != nullptr)
            curr = curr->right.get();

        return curr->key;
    }

    /**
     * @brief Returns the true if the key is in the tree by performing binary search
     * @post the state of the tree remain unchanged
     * @return true if the key is in the tree, false otherwise
     * @note Time Complexity: O(h) where h is the tree height
     * @note Space Complexity: O(1)
     */
    template<typename Key, typename Compare>
    bool BST<Key, Compare>::contains(const Key& k) const{
        node curr = root_.get(); // get() returns the raw Node* stored inside root_ without transferring ownership(unique_ptr)
        while(curr != nullptr){
            if (!compare_(k, curr->key) && !compare_(curr->key, k))
                return true;
            if (compare_(k, curr->key))
                curr = curr->left.get();
            else
                curr = curr->right.get();
        }
        return false;
    }


    //AUX function for validate()
    template<typename Key, typename Compare>
    bool BST<Key,Compare>::is_node_valid(node n, const Key* min_key, const Key* max_key) const {
        if (n == nullptr)
            return true;

        if (min_key != nullptr && !compare_(*min_key, n->key))
            return false;

        if (max_key != nullptr && !compare_(n->key, *max_key))
            return false;

        return is_node_valid(n->left.get(), min_key, &(n->key)) &&
               is_node_valid(n->right.get(), &(n->key), max_key);
    }
    /**
     * @brief check wether or not the BST is valid
     * @return true if the BST is valid, false otherwise
     * @post the state of the BST remain unchanged
     */
    template <typename Key, typename Compare>
    bool BST<Key,Compare>::validate() const {
        return is_node_valid(root_.get(), nullptr, nullptr);
    }

    /**
     * @brief Returns the successor of the specified key in the BST.
     *
     * The successor is the smallest key strictly greater than the given key.
     *
     * @param key The key whose successor has to be found.
     * @return The successor key if it exists, std::nullopt otherwise.
     *
     * @post The state of the tree remains unchanged.
     *
     * @note Time complexity: O(h), where h is the height of the tree.
     * @note Space complexity: O(1).
    */
    template<typename Key, typename Compare>
    std::optional<Key> BST<Key,Compare>::successor(const Key& key) const {
        //getting the node from the key
        node curr = root_.get(); // get() returns the raw Node* stored inside root_ without transferring ownership(unique_ptr)
        while(curr != nullptr){
            if (!compare_(key, curr->key) && !compare_(curr->key, key))
                break;
            if (compare_(key, curr->key))
                curr = curr->left.get();
            else
                curr = curr->right.get();
        }
        if(curr == nullptr){
            return std::nullopt;
        }
        //If the right subtree exists the successor is its minimum node.
        if (curr->right != nullptr) {
            curr = curr->right.get();

            while (curr->left != nullptr)
                curr = curr->left.get();

            return curr->key;
        }

        //Otherwise, go upward until we find an ancestor for which curr belongs to the left subtree.
        node parent = curr->parent;
        while(parent != nullptr && curr == parent->right.get()){
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
    template <typename Key, typename Compare>
    std::optional<Key> BST<Key,Compare>::predecessor(const Key& key) const{
        //let's find the node: 
        node temp = root_.get();
        while(temp != nullptr){
            if(!compare_(temp->key, key) && !compare_(key,temp->key)){
                break;
            }
            if(compare_(key,temp->key))
                temp = temp->left.get();
            else
                temp = temp->right.get();
        }
        if(temp == nullptr)
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
        while (parent != nullptr &&
            curr == parent->left.get()) {

            curr = parent;
            parent = parent->parent;
        }

        // If parent is nullptr, no predecessor exists.
        if (parent == nullptr)
            return std::nullopt;

        return parent->key;
    }
    
    /**
     * @brief Classic algorithm for deleting a node in a BST.
     *
     * Handles the three standard cases:
     *  - node with no children;
     *  - node with one child;
     *  - node with two children, replacing its key with its successor's key.
     *
     * @param n Pointer to the node to delete.
     * @return true if the node was deleted, false if n is nullptr.
     *
     * @note Time complexity: O(h), where h is the height of the tree.
     * @note Space complexity: O(1).
     */
    template<typename Key, typename Compare>
    bool BST<Key, Compare>::delete_node(node n) {

        if (n == nullptr)
            return false;

        // CASE 1: leaf node.
        if (n->left == nullptr && n->right == nullptr) {

            if (n->parent == nullptr) {
                root_.reset();
            }
            else if (n->parent->left.get() == n) {
                n->parent->left.reset();
            }
            else {
                n->parent->right.reset();
            }

            --size_;
            return true;
        }

        // CASE 2: only right child.
        if (n->left == nullptr) {

            node parent = n->parent;
            std::unique_ptr<Node> child = std::move(n->right);

            child->parent = parent;

            if (parent == nullptr) {
                root_ = std::move(child);
            }
            else if (parent->left.get() == n) {
                parent->left = std::move(child);
            }
            else {
                parent->right = std::move(child);
            }

            --size_;
            return true;
        }

        // CASE 2: only left child.
        if (n->right == nullptr) {

            node parent = n->parent;
            std::unique_ptr<Node> child = std::move(n->left);

            child->parent = parent;

            if (parent == nullptr) {
                root_ = std::move(child);
            }
            else if (parent->left.get() == n) {
                parent->left = std::move(child);
            }
            else {
                parent->right = std::move(child);
            }

            --size_;
            return true;
        }

        // CASE 3: two children.
        node successor = n->right.get();

        while(successor->left != nullptr){
            successor = successor->left.get();
        }
        n->key = successor->key;

        return delete_node(successor);
    }


    /**
     * @brief Recomputes the deepest node and the height of the BST.
     *
     * Performs a DFS starting from the given node, keeping track of the
     * current depth. Whenever a node deeper than the current maximum is found,
     * height_ and deepest_node_ are updated.
     *
     * @param n Current node being visited.
     * @param height Depth of the current node.
     *
     * @note Time complexity: O(n), since every node may be visited once.
     * @note Space complexity: O(h), due to the recursive call stack.
     */
    template<typename Key, typename Compare>
    void BST<Key, Compare>::recompute_deepest(node n, size_type height) {

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
     * @brief Delete the passed node and returns true if correctly deleted 
     * @param n is the node to delete
     * @return true if correctly deleted, false otherwise
     * @post delete the passed node and fix the tree if necessary. The tree must contain the same nodes, except for n. 
     * @note Time Complexity: O(n) worst case(has to recompute deepest). O(logn) avg.
     * @note Space Complexity: O(1)
     * @note if the node to delete is in the same path of the deepest node(which defines the tree height), it must update the tree heigth 
     */
    template<typename Key, typename Compare>
    bool BST<Key,Compare>::del(const Key& n){
        //getting the node from the key
        node to_del = root_.get();
        while(to_del != nullptr){
            if (!compare_(n, to_del->key) && !compare_(to_del->key, n))
                break;
            if (compare_(n, to_del->key))
                to_del = to_del->left.get();
            else
                to_del = to_del->right.get();
        }
        if(to_del == nullptr)
            return false;

        node temp = deepest_node_;
        while (temp != nullptr)
        {
            if(temp == to_del){ //if the node to delete is in the path of the deepest node, delete it and recalculate the deepest node
                bool res = delete_node(to_del);
                height_ = 0;
                deepest_node_ = nullptr;
                recompute_deepest(root_.get(), 1);
                return res;
            }
            temp = temp->parent;
        }
        return delete_node(to_del);
    }
    

}