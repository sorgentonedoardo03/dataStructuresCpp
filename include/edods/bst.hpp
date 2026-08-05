#pragma once // see the eof to see an explanation

#include <cstddef>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

/* 
 * ------- NAMESPACE IN C++ ---------
 * in C++ a namespace is a logical container used to prevent naming conflicts, 
 * allowing you to reuse the exact same class, function, or variable names in different 
 * contexts within the same project.
*/
namespace edods{
    /*
    * ----- GENERIC TYPES ------
    * this tells the compiler that the class declared after will depends on these 2 generic types.
    * Key and Compare are just placeholders. typename is the same of "class" keyword, but actually
    * it is used for clarity inside templates. 
    * Dependent Qualified Names: if you use T::member inside the class,use 'typename' before T::member 
    * because C++ assumes it's a value by default, not a type.
    * Furthermore, templates accept also constant: template <typename T, std::size_t N> here N is a const.
    */
    template< typename Key, typename Compare = std::less<Key> >
    class BST{

        // using allow you to assign an alias to a certain type: 
        using key_type = Key; //now Key can be also used with the name key_type
        using size_type = std::size_t; // and std::size_t can be just used with size_type

    }

}



/*
 * --------------- PRAGMA ONCE ---------------------
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
*/
