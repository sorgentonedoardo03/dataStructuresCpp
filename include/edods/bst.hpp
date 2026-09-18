#pragma once // see the eof to see an explanation

#include <cstddef>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

namespace edods{

   /**
     * @brief Generic unbalanced binary search tree storing unique keys.
     *
     * Keys are ordered using the comparison object provided through the
     * Compare template parameter. By default, std::less<Key> is used.
     *
     * The tree supports insertion, deletion, lookup, minimum, maximum,
     * predecessor, successor, validation, and snapshot persistence through
     * save() and load().
     *
     * Average complexity for search, insertion, and deletion: O(log n).
     * Worst-case complexity: O(n), since the tree is not self-balancing.
     *
     * @tparam Key
     *     Type of the keys stored in the tree.
     *
     * @tparam Compare
     *     Callable type defining a strict weak ordering between keys.
     *     compare(a, b) must return true when a must precede b.
     */
    template< typename Key, typename Compare = std::less<Key> >
    class BST{
        public: 
            // using allow you to assign an alias to a certain type: 
            using key_type = Key; //now Key can be also used with the name key_type
            using size_type = std::size_t; // and std::size_t can be just used with size_type

            BST() = default; // it creates the default class constructor, BST(){}

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
            static BST load(const std::filesystem::path& path); //static method belongs to the class itself rather than an instance, meaning it can be called without instantiating the class.
            
        private:
            struct Node {
                explicit Node(const Key& value, Node* parent_node = nullptr) 
                    : key(value), parent(parent_node) {} // struct constructor. 

                Key key;
                std::unique_ptr<Node> left;  // std::unique_ptr<T> exclusively owns a dynamically allocated object and automatically destroys it when the pointer goes out of scope.
                std::unique_ptr<Node> right;
                Node* parent = nullptr;
            };
            
            using node = Node*;
            bool delete_node(node n);
            void recompute_deepest(node n, int height);
            std::unique_ptr<Node> root_;
            bool is_node_valid(node n) const;
            size_type size_ = 0;
            size_type height_ = 0; 
            Node* deepest_node_ = nullptr;
            Compare compare_;
            
    
    };

}

#include "detail/bst.tpp" //in bst.tpp we will write the implementation of the prototypes defined above

/* Here are some C++ important concepts for understanding this header(a declaration of all methods and class members, without implementation): 
==========================================================
 * --------------- PRAGMA ONCE ---------------------
==========================================================
 * It is used to avoid re-declarations during compilation phase. Let's look at this example.
 * Let's say you have this 3 files: 
 1. Persona.h 
    struct Persona {
        std::string nome;
    };

 2. Studente.h (Includes Persona.h)
    #include "Persona.h"
    struct Studente {
        Persona p;
        int matricola;
    };

3. main.cpp (includes both Persona.h and Studente.h)
    #include "Persona.h"
    #include "Studente.h" 

    int main() {
        return 0;
    }

* Without '#pragma once' in Persona.h, the preprocessor copies Persona.h into Studente.h.
* Then in main.cpp, Persona.h is included again along with Studente.h, causing Persona 
* to be declared twice and resulting in a redefinition error.
* The original directive for this in c++ was not pragma once, but was something called
* Macro Guard, here's an example: 

#ifndef PERSONA_H
#define PERSONA_H

struct Persona {
    std::string nome;
};

#endif // PERSONA_H
* now in modern compilers you can use just #pragma once !

==========================================================
* ------------------- GENERIC TYPES ----------------------
==========================================================
template< typename Key, typename Compare = std::less<Key> >
    * this tells the compiler that the class declared after will depends on these 2 generic types.
    * Key and Compare are just placeholders. typename is the same of "class" keyword, but actually
    * it is used for clarity inside templates. 
    * Dependent Qualified Names: if you use T::member inside the class,use 'typename' before T::member 
    * because C++ assumes it's a value by default, not a type.
    * Furthermore, templates accept also constant: template <typename T, std::size_t N> here N is a const.
    
==========================================================
* --------------------- OPTIONALS ------------------------
==========================================================
* std::optional<T> is a wrapper type used when a value may or may not be present.
*
* It can contain either:
*   - a value of type T
*   - no value,  std::nullopt
* For example 
   std::optional<int> test = 100;
*
* Here, test contains the integer value 100.
* We can later remove the contained value by assigning std::nullopt:
   test = std::nullopt;
*
* After this assignment, test does not contain an int anymore.
*
* std::optional<T> can also be used as the return type of a function when
* the function may fail to produce a meaningful result.
*
* Example:
*
   std::optional<int> findValue(...);
*
* The caller can then check whether a value is present before using it.
* An optional can also be default-constructed:
   std::optional<int> value;
*
* In this case, the optional starts in the "empty" state, meaning that it
* does not contain a value. Conceptually this is equivalent to initializing
* it with std::nullopt:
*
   std::optional<int> value = std::nullopt;
*
* Note: std::optional does not literally store a "null value" of type T.
* It stores either an object of type T or nothing at all.
* To return the content of the optional use the method value():
    std::optional<int> ciao = 10
    std::cout<< ciao.value()
* Or if you want to check if it contans a value
    if(ciao.has_value() ) ...
* or 
    if(ciao) ...
* To print directly the value you have to use the pointers syntax: 
    std::cout << *ciao

==========================================================
* --------------------- EXPLICIT ------------------------
==========================================================
* explicit is a keyword used with constructors and conversion operators
* to prevent unwanted implicit conversions.
*
* Without explicit, C++ may automatically convert a value of another type
* into an object of our class if a compatible constructor exists. Here's an example:
    class Test {
    public:
        Test(int value) {
            ...
        }
    };
* Since the constructor takes a single int, C++ can use it as an implicit
* conversion from int to Test.
*
* This means that code like:
    Test t = 10;
*
* is allowed. Conceptually, the compiler transforms it into something similar to:
*   Test t(10);
* The same implicit conversion can also happen when passing arguments to functions:
*
    void print(Test t);
    print(10);   // 10 is automatically converted into Test(10)
*
* If we write the constructor as explicit:

   class Test {
   public:
       explicit Test(int value) {
           ...
       }
   };
* then implicit conversions are disabled.
* This is NOT allowed anymore:
   Test t = 10;    // error
   print(10);      // error

==========================================================
* --------------------- NAMESPACE ------------------------
==========================================================
* in C++ a namespace is a logical container used to prevent naming conflicts, 
* allowing you to reuse the exact same class, function, or variable names in different 
* contexts within the same project.





*/
