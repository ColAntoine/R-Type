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
	-find . -type f -name '*.o' -de    lete

fclean:
	@echo "Removing lib directory, arcade binary, and build directory..."
	rm -rf lib arcade build

re: fclean all