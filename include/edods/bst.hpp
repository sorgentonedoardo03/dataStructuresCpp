#pragma once // 

#include <cstddef>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

// in C++ a namespace is a logical container used to prevent naming conflicts, allowing you to reuse the exact same class, function, or variable names in different contexts within the same project.
namespace edods{
    //this tells the compiler that the class declared after will depends on these 2 generic types
    template< typename Key, typename Compare = std::less<Key> >
    class BST{

    }

}