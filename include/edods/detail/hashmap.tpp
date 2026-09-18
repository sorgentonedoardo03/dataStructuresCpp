#include <filesystem>
#include <fstream>
#include <optional>

namespace edods{
    /**
     * @brief Checks whether the hash map is empty.
     *
     * @return true if the hash map contains no elements, false otherwise.
     *
     * @post The state of the hash map remains unchanged.
     *
     * @note Time complexity: O(1).
     * @note Space complexity: O(1).
     */
    template<typename Value>
    [[nodiscard]] bool HashMap<Value>::empty() const noexcept{
        return size_==0;
    }

    /**
     * @brief Returns the number of elements stored in the hash map.
     *
     * @return The current number of key-value pairs in the hash map.
     *
     * @post The state of the hash map remains unchanged.
     *
     * @note Time complexity: O(1).
     * @note Space complexity: O(1).
     */
    template<typename Value>
    typename HashMap<Value>::size_type HashMap<Value>::size() const{
        return size_;
    }

    /**
     * @brief Computes the current load factor of the hash map.
     *
     * @return The ratio of stored elements to total bucket capacity (size_ / buckets_.size()).
     *
     * @post The state of the hash map remains unchanged.
     *
     * @note Time complexity: O(1).
     * @note Space complexity: O(1).
     */
    template<typename Value>
    float HashMap<Value>::load_perc() const{
        return static_cast<float>(size_)/buckets_.size();
    }

    /**
     * @brief Returns the total number of buckets in the hash map.
     *
     * @return size_type The capacity of the bucket array.
     *
     * @post The state of the hash map remains unchanged.
     *
     * @note Time complexity: O(1).
     * @note Space complexity: O(1).
     */
    template<typename Value>
    [[nodiscard]] typename HashMap<Value>::size_type HashMap<Value>::bucket_count() const noexcept{
        return buckets_.size();
    }

    /**
     * @brief Generates a 64-bit hash for the given string key using the djb2 algorithm.
     *  Source: Stack Overflow
     * @param key The string key to be hashed.
     * @post The state of the HashMap remains unaltered (pure function).
     * @return size_type The computed 64-bit unsigned hash value.
     *
     * @note Implements Daniel J. Bernstein's classic djb2 algorithm:
     *       hash = (hash * 33) + c, initialized with seed 5381.
     *       Characters are treated as unsigned to prevent sign extension issues.
     */
    template<typename Value>
    typename HashMap<Value>::size_type HashMap<Value>::hash(const Key& key) const noexcept {
        size_type hash_value = 5381;
        for (unsigned char c : key) {
            // Equivalent to: hash_value = hash_value * 33 + c;
            hash_value = ((hash_value << 5) + hash_value) + static_cast<size_type>(c);
        }
        return hash_value;
    }

    /**
     * @brief insert a new node with value v , with key key
     * @post buckets_ must have the same elements and the new node
     * @return true if the insert was successful, false otherwise
     */
    template<typename Value>
    bool HashMap<Value>::insert(const Key& key, const Value& v){
        size_type digest = hash(key) % buckets_.size();
        float lf = static_cast<float>(size_)/buckets_.size();
        if(buckets_[digest] == nullptr && lf <= MAX_LOAD_FACTOR ){
            buckets_[digest] = std::make_unique<Node>(key, v);
            ++size_;
            return true;
        }
        else if(buckets_[digest] != nullptr && lf <= MAX_LOAD_FACTOR){
            node temp = buckets_[digest].get();
            while(temp != nullptr){
                //duplicate key
                if(temp->key == key)
                    return false;
                temp = temp->next.get();
            }
            buckets_[digest] = std::make_unique<Node>(key, v, std::move(buckets_[digest]));
            ++size_;
            return true;
        }
        else{
            // Rehash needed, load factor exceeded threshold
            rehash(buckets_.size() * 2);
            return insert(key, v);
        }
    }

    /**
     * @brief Rehashes the entire hash map into a newly sized bucket array.
     *
     * Reallocates the bucket array to new_capacity and redistributes all existing
     * nodes based on their new modulo indices.
     *
     * @param new_capacity The new number of buckets.
     *
     * @post buckets_.size() == new_capacity.
     * @post size_ remains unchanged.
     *
     * @note Time complexity: O(N + M) where N is size_ and M is current bucket count.
     * @note Space complexity: O(new_capacity).
     */
    template<typename Value>
    void HashMap<Value>::rehash(size_type new_capacity){
        std::vector<std::unique_ptr<Node>> new_buckets(new_capacity);

        for(auto& head : buckets_){
            while(head != nullptr){
                std::unique_ptr<Node> current = std::move(head);
                head = std::move(current->next);

                size_type new_digest = hash(current->key) % new_capacity;

                current->next = std::move(new_buckets[new_digest]);
                new_buckets[new_digest] = std::move(current);
            }
        }

        buckets_ = std::move(new_buckets);
    }

    /**
     * @brief Deletes a key-value pair from the hash map by key.
     *
     * @param key The key of the element to delete.
     * @return true if the element was found and deleted, false otherwise.
     *
     * @post The size of the hash map is decreased by 1 if deletion succeeded.
     *
     * @note Average time complexity: O(1).
     * @note Worst-case time complexity: O(n).
     * @note Space complexity: O(1).
     */
    template<typename Value>
    bool HashMap<Value>::del(const Key& key){
        if(empty()){
            return false;
        }

        size_type digest = hash(key) % buckets_.size();
        if(buckets_[digest] == nullptr){
            return false;
        }

        // If the key is in the head node of the bucket
        if(buckets_[digest]->key == key){
            buckets_[digest] = std::move(buckets_[digest]->next);
            --size_;
            return true;
        }

        // Otherwise search in the rest of the bucket list
        node prev = buckets_[digest].get();
        while(prev->next != nullptr){
            if(prev->next->key == key){
                prev->next = std::move(prev->next->next);
                --size_;
                return true;
            }
            prev = prev->next.get();
        }

        return false;
    }

    /**
     * @brief Retrieves the value associated with the specified key.
     *
     * @param key The key to search for.
     * @return std::optional<Value> containing the value if found, std::nullopt otherwise.
     *
     * @post The state of the hash map remains unchanged.
     *
     * @note Average time complexity: O(1).
     * @note Worst-case time complexity: O(n).
     * @note Space complexity: O(1).
     */
    template<typename Value>
    [[nodiscard]] std::optional<Value> HashMap<Value>::get(const Key& key) const{
        if(empty()){
            return std::nullopt;
        }

        size_type digest = hash(key) % buckets_.size();
        if(buckets_[digest] == nullptr){
            return std::nullopt;
        }

        node temp = buckets_[digest].get();
        while(temp != nullptr){
            if(temp->key == key){
                return temp->value;
            }
            temp = temp->next.get();
        }

        return std::nullopt;
    }

    /**
     * @brief Removes all elements from the hash map.
     *
     * Clears all buckets and resets size to 0 while maintaining the current bucket capacity.
     *
     * @post size() == 0.
     *
     * @note Time complexity: O(N) where N is the bucket count.
     * @note Space complexity: O(1).
     */
    template<typename Value>
    void HashMap<Value>::clear() noexcept{
        for(auto& bucket : buckets_){
            bucket.reset();
        }
        size_ = 0;
    }

    /**
     * @brief Serializes the HashMap to persistent storage at the specified path.
     *
     * Writes the total size as a header followed by each (key, value) pair in binary format.
     *
     * @param path Filesystem path where the binary dump will be written.
     * @throws std::runtime_error if the destination file cannot be opened or writing fails.
     *
     * @note Time complexity: O(N + M) where N is size_ and M is bucket count.
     * @note Space complexity: O(1).
     */
    template<typename Value>
    void HashMap<Value>::save(const std::filesystem::path& path) const{
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if(!out.is_open()){
            throw std::runtime_error("Error opening the file: " + path.string());
        }

        // Header: writing the size of HashMap
        out.write(reinterpret_cast<const char*>(&size_), sizeof(size_type));

        // Writing key-value pairs
        for(const auto& head : buckets_){
            node curr = head.get();
            while(curr != nullptr){
                // 1. Write key string length + characters
                size_type key_len = curr->key.size();
                out.write(reinterpret_cast<const char*>(&key_len), sizeof(size_type));
                out.write(curr->key.data(), key_len);

                // 2. Write value
                out.write(reinterpret_cast<const char*>(&(curr->value)), sizeof(Value));

                curr = curr->next.get();
            }
        }

        out.flush();
        if(!out.good()){
            throw std::runtime_error("I/O error while writing data to file: " + path.string());
        }
    }

    /**
     * @brief Deserializes a HashMap from persistent storage.
     *
     * Reads the serialized binary file, extracts the total size, and sequentially
     * inserts each (key, value) pair into a newly constructed HashMap.
     *
     * @param path Filesystem path of the binary dump to load.
     * @return HashMap<Value> A newly reconstructed HashMap instance.
     * @throws std::runtime_error if the file cannot be opened or if I/O read fails.
     *
     * @note Time complexity: O(N) average.
     * @note Space complexity: O(N) to allocate the reconstructed nodes.
     */
    template<typename Value>
    HashMap<Value> HashMap<Value>::load(const std::filesystem::path& path){
        std::ifstream in(path, std::ios::binary);
        if(!in.is_open()){
            throw std::runtime_error("Error opening the file for reading: " + path.string());
        }

        // 1. Read size from header
        size_type saved_size = 0;
        in.read(reinterpret_cast<char*>(&saved_size), sizeof(size_type));
        if(!in.good()){
            throw std::runtime_error("Error reading header from file: " + path.string());
        }

        HashMap<Value> map;

        // 2. Read each key-value pair and insert into the map
        for(size_type i = 0; i < saved_size; ++i){
            size_type key_len = 0;
            in.read(reinterpret_cast<char*>(&key_len), sizeof(size_type));

            std::string k(key_len, '\0');
            in.read(&k[0], key_len);

            Value v{};
            in.read(reinterpret_cast<char*>(&v), sizeof(Value));

            if(!in.good()){
                throw std::runtime_error("Corrupted file or unexpected EOF while reading elements: " + path.string());
            }

            map.insert(k, v);
        }

        return map;
    }

}