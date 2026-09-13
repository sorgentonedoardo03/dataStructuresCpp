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

    
}