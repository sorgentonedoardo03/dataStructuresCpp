all: 
	cmake -B build
	cd build && make
	./build/tests/bst
	./build/tests/hashmap
	./build/tests/rb
	./build/tests/queue
clean:
	rm -rf build

.PHONY: clean all