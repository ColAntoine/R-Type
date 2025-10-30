##
## EPITECH PROJECT, 2025
## Makefile
## File description:
## makefile for easier compilation
##

.PHONY: all build clean fclean re pang

all: build

build:
	mkdir -p build && cd build && cmake .. && cmake --build . && cp -r bin/* ../

clean:
	@echo "Removing object files (.o)..."
	-find . -type f -name '*.o' -delete

fclean:
	@echo "Removing lib directory, rtype binary, and build directory..."
	rm -rf lib rtype build r-type_client r-type_server lib pang
	cd Games/Pang && make fclean

re: fclean all

pang:
	@echo "Building Pang game..."
	cd Games/Pang && $(MAKE)
	@if [ -f Games/Pang/build/pang ]; then \
		cp Games/Pang/build/pang .; \
		cp -r Games/Pang/build/libECS.so . 2>/dev/null || true; \
		echo "Pang game built successfully! Run with: ./pang"; \
	fi

test: build
	cd build && ctest --output-on-failure