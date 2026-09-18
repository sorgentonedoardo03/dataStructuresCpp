#pragma once
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>

namespace edods{
    template<typename Value>
    class Queue{
        public: 
            using size_type = size_t;
            [[nodiscard]] bool enqueue(const Value& v);
            void clear() noexcept;
            std::optional<Value> dequeue();
            size_type size() const;
            void save(const std::filesystem::path& path) const;
            static Queue load(const std::filesystem::path& path);
            
        private:
            struct Node{
                std::unique_ptr<Node> next; 
                Value value;
                explicit Node(const Value& v, std::unique_ptr<Node> n = nullptr): next(std::move(n)), value(v) {}
            };
            using node = Node*;
            size_type size_ = 0;
            std::unique_ptr<Node> head_;
            node tail_; //it cannot be unique_ptr, because it would violate the unique property
    };
}

#include "detail/queue.tpp"