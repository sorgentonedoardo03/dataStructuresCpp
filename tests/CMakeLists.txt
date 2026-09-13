cmake_minimum_required(VERSION 4.4)
add_executable(
    bst_operations_test
    bst_operations_test.cpp
)

target_link_libraries(
    bst_operations_test
    PRIVATE
    edods
)

add_test(
    NAME bst_operations
    COMMAND bst_operations_test
)