add_executable(bst bst.cpp)
add_executable(rb rb.cpp)
add_executable(hashmap hashmap.cpp)
add_executable(queue queue.cpp)

target_link_directories(queue PRIVATE edods)
target_link_directories(bst PRIVATE edods)
target_link_directories(rb PRIVATE edods)
target_link_directories(hashmap PRIVATE edods)