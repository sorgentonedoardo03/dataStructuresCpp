
namespace edods{
    /**
     * @brief Returns the number of elements stored in the queue.
     *
     * This operation does not modify the queue.
     *
     * @return The current number of elements in the queue.
     *
     * @post The state of the queue remains unchanged.
     *
     * @note Time Complexity: O(1)
     * @note Space Complexity: O(1)
     */
    template<typename Value>
    typename Queue<Value>::size_type Queue<Value>::size() const{
        return size_;
    }

    /**
     * @brief Inserts a new element at the end of the queue.
     *
     * Creates a new node with the specified value and appends it to the tail.
     * If the queue is empty, the new node becomes both head and tail.
     *
     * @param v Value to insert in the queue.
     * @return true if the insertion was successful.
     *
     * @post size() is incremented by 1.
     * @post The new element becomes the tail of the queue.
     *
     * @note Time Complexity: O(1)
     * @note Space Complexity: O(1)
     */
    template<typename Value>
    [[nodiscard]] bool Queue<Value>::enqueue(const Value& v){
        std::unique_ptr<Node> new_node = std::make_unique<Node>(v);
        //if is not the first node
        if(head_ != nullptr){
            tail_->next = std::move(new_node); //have to use move otherwise at the end of func. exec. new_node would be destroyed(and so also the new node...)
            //move transfer also the property of new_node
            tail_ = tail_->next.get();
        }
        //if it is the first node
        else
        {
            head_ =  std::move(new_node);
            tail_ = head_.get();
        }
        ++size_;
        return true;
    }

    /**
     * @brief Removes and returns the front element of the queue.
     *
     * Extracts the value from the head of the queue and advances the head pointer.
     *
     * @return std::optional<Value> containing the front element, or std::nullopt if the queue is empty.
     *
     * @post If the queue is not empty, size() is decremented by 1.
     * @post The old head node is destroyed and removed from the queue.
     *
     * @note Time Complexity: O(1)
     * @note Space Complexity: O(1)
     */
    template<typename Value>
    std::optional<Value> Queue<Value>::dequeue(){
        if(head_ == nullptr)
            return std::nullopt;
        std::optional<Value> n=head_->value;
        if(head_->next == nullptr){
            tail_ = nullptr;
            head_ = nullptr;
            --size_;
            return n;
        }
        head_ = std::move(head_->next);
        --size_;

        return n;
    }
    
    /**
     * @brief Removes all elements from the queue, leaving it empty.
     *
     * Destroys all nodes in the queue and resets head, tail, and size.
     *
     * @post size() == 0
     * @post head_ == nullptr
     * @post tail_ == nullptr
     *
     * @note Time Complexity: O(n), where n is the number of elements in the queue
     * @note Space Complexity: O(1)
     */
    template<typename Value>
    void Queue<Value>::clear() noexcept {
        while (head_ != nullptr) {
            head_ = std::move(head_->next);
        }
        
        tail_ = nullptr;
        size_ = 0;
    }

    /**
     * @brief Serializes the Queue to persistent storage at the specified path.
     *
     * Writes the total size as a header followed by each element in FIFO order in binary format.
     *
     * @param path Filesystem path where the binary dump will be written.
     * @throws std::runtime_error if the destination file cannot be opened or writing fails.
     *
     * @note Time Complexity: O(n), where n is the number of elements in the queue.
     * @note Space Complexity: O(1).
     */
    template<typename Value>
    void Queue<Value>::save(const std::filesystem::path& path) const {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) {
            throw std::runtime_error("Error opening the file: " + path.string());
        }

        // Header: writing the size of Queue
        out.write(reinterpret_cast<const char*>(&size_), sizeof(size_type));

        // Writing elements in FIFO order (from head to tail)
        node curr = head_.get();
        while (curr != nullptr) {
            out.write(reinterpret_cast<const char*>(&(curr->value)), sizeof(Value));
            curr = curr->next.get();
        }

        out.flush();
        if (!out.good()) {
            throw std::runtime_error("I/O error while writing data to file: " + path.string());
        }
    }

    /**
     * @brief Deserializes a Queue from persistent storage.
     *
     * Reads the serialized binary file, extracts the total size, and sequentially
     * enqueues each element to reconstruct the original queue.
     *
     * @param path Filesystem path of the binary dump to load.
     * @return Queue<Value> A newly reconstructed Queue instance.
     * @throws std::runtime_error if the file cannot be opened or if I/O read fails.
     *
     * @note Time Complexity: O(n), where n is the number of elements in the file.
     * @note Space Complexity: O(n) to allocate the reconstructed nodes.
     */
    template<typename Value>
    Queue<Value> Queue<Value>::load(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) {
            throw std::runtime_error("Error opening the file for reading: " + path.string());
        }

        // 1. Read size from header
        size_type saved_size = 0;
        in.read(reinterpret_cast<char*>(&saved_size), sizeof(size_type));
        if (!in.good()) {
            throw std::runtime_error("Error reading header from file: " + path.string());
        }

        Queue<Value> queue;

        // 2. Read each element and enqueue it
        for (size_type i = 0; i < saved_size; ++i) {
            Value v{};
            in.read(reinterpret_cast<char*>(&v), sizeof(Value));

            if (!in.good()) {
                throw std::runtime_error("Corrupted file or unexpected EOF while reading elements: " + path.string());
            }

            static_cast<void>(queue.enqueue(v));
        }

        return queue;
    }

}
