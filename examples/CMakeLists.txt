# we don't have to specify again the C++ standard and the other configuration if the target uses edods library 
# because they will inherit the properties from edods. Instead if we add a target that does not use edods, 
# we have to define again the configuration. If we had used PRIVATE in the root CMakeList it wouldn't  
# have inherited the properties
add_executable(esempio main.cpp) 
target_link_directories(esempio PRIVATE edods) 
# With an executable there's nothing downstream, so PRIVATE is enough; PUBLIC would work but is unnecessary