all: 
	cmake -B build
clean:
	rm -rf build

.PHONY: clean all