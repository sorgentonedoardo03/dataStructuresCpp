#pragma once
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace edods{
    template<typename Value>
    class HashMap{
        public:
            using Key = std::string;
            using mapped_type = Value;
            using size_type = std::size_t;

            bool insert(const Key& key,const Value& v);
            bool del(const Key& key);
            [[nodiscard]] std::optional<Value> get(const Key& key) const;
            [[nodiscard]] bool empty() const noexcept;   
            [[nodiscard]] size_type bucket_count() const noexcept;

            size_type size() const;
            float load_perc() const;
            void clear() noexcept;
            void save(const std::filesystem::path& path) const;
            static HashMap load(const std::filesystem::path& path);

            explicit HashMap(size_type initial_buckets = 100) 
            : buckets_(initial_buckets) {}
            

        private: 
            struct Node{
                Value value;
                std::string key;
                std::unique_ptr<Node> next;
                Node(Key k , Value v, std::unique_ptr<Node> n = nullptr): 
                key(std::move(k)), value(std::move(v)), next(std::move(n)){}
            };

            //note: unique_prt is not copyable so the entire HashMap class as well
            //if we try to assign hmap1 = hmap2 where hmap1,hmap2 are 2 instances ,we
            // would get a "Copy constructor is implict deleted" error, the compiler will block it
            std::vector<std::unique_ptr<Node>> buckets_;
            
            using node = Node*;
            // constexpr ensures that a variable or function is evaluated at compile time for maximum runtime performances
            static constexpr float MAX_LOAD_FACTOR = 0.75f; // threshold
            void rehash(size_type new_capacity);

            size_type size_ = 0;
            size_type hash(const std::string& Key) const noexcept;//in C++ string& Key means passing a parameter for reference
    };
}

#include "detail/hashmap.tpp"