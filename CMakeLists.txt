cmake_minimum_required(VERSION 4.4)

project(edods LANGUAGES CXX)

add_library(edods INTERFACE)

target_include_directories(
    edods
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(edods INTERFACE cxx_std_17)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()