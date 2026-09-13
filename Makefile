CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude

bst_test: tests/bst_operations_test.cpp include/edods/bst.hpp
	$(CXX) $(CXXFLAGS) tests/bst_operations_test.cpp -o bst_test

clean:
	rm -f bst_test

.PHONY: clean