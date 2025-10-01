##
## EPITECH PROJECT, 2025
## Makefile
## File description:
## makefile for easier compilation
##

.PHONY: all build clean fclean re

all: build

build:
	mkdir -p build && cd build && cmake .. && cmake --build .

clean:
	@echo "Removing object files (.o)..."
	-find . -type f -name '*.o' -delete

fclean:
	@echo "Removing lib directory, rtype binary, and build directory..."
	rm -rf lib rtype build

re: fclean all

test: build
	cd build && ctest --output-on-failure