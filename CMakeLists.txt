# CMake describes the project and generates the native build files. In this setup it ends up producing Makefiles(f.e.) on your system
cmake_minimum_required(VERSION 4.4)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(ds_lib LANGUAGES CXX) # creates a CMake project called "ds_lib" with C++ language.It automatically identifies the C++ compiler installed on the machine.

# create a target library called edods 
# note: a target can be also an executable for example(add_executable(..))... we can have multiple targets.
# edods will be the name that you will use in the other instructions such as target_include_directories.
add_library(edods INTERFACE) 

# the following instruction tells the compiler where to find edods library's headers  
# now CMake expects to find another CMakeLists.txt in /test directory 
# with interface, we say this include path is for targets that use edods, not for edods itself
# so consumers inherit it automatically. 
target_include_directories(
    edods
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
) 

# Propagate the C++17 requirement to consumers via INTERFACE
target_compile_features(edods INTERFACE cxx_std_17) 

add_subdirectory(examples) # CMake will execute the CMakeList in examples folder 

# include loads modules and functions in CMake 
include(CTest)

if(BUILD_TESTING) # This variable is imported from CTest module
    add_subdirectory(tests) # tells CMake to execute the CMakeList inside the /tests folder. It doesn't declare a new target
endif()



# ---- PRIVATE, PUBLIC, INTERFACE ----
# In CMake, PRIVATE, PUBLIC, and INTERFACE define where a property applies. PRIVATE means this is only needed to build this target and is not passed 
# on to dependents. PUBLIC means it's needed to build this target and should also be passed on to anything that links against it. INTERFACE means 
# it's not used to build this target at all, it is propagated to dependents. In our case, edods is header-only, 
# so INTERFACE is correct: we are saying consumers of edods must add this include directory and use at least C++17.

# --------  WHY INTERFACE  ? ------------
# the include directory is associated with the edods target, but edods itself does not need that directory in order to be built. 
# Instead, that directory is required by any target that uses edods. INTERFACE expresses exactly this distinction: what is needed to 
# build the target itself versus what is needed by targets that use it. Since edods is an INTERFACE/header-only library, it has no compilation step of its own.
# Therefore, properties such as the include directory are usage requirements: they are stored on edods and propagated to targets that depend on it.

