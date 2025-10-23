# Makefile for Pang Game
# Simple wrapper around CMake for easy building

.PHONY: all build clean re fclean test run

# Default target
all: build

# Build the project
build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build . -j$$(nproc)
	@if [ -d build/bin ]; then cp -r build/bin/* .; fi
	@echo "Pang game built successfully!"

# Clean build artifacts
clean:
	@rm -rf build
	@rm -f pang
	@rm -rf lib
	@echo "Build artifacts cleaned"

# Full clean (including CMake cache)
fclean: clean
	@find . -name "CMakeCache.txt" -delete
	@find . -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "Full clean completed"

# Rebuild everything
re: fclean all

# Run the game
run: build
	@if [ -f pang ]; then \
		./pang; \
	else \
		echo "Error: pang executable not found. Build failed?"; \
		exit 1; \
	fi

# Run with debugging
debug: build
	@if [ -f pang ]; then \
		gdb ./pang; \
	else \
		echo "Error: pang executable not found. Build failed?"; \
		exit 1; \
	fi

# Help target
help:
	@echo "Pang Game - Makefile commands:"
	@echo "  make [all]   - Build the game"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make fclean  - Full clean (including CMake cache)"
	@echo "  make re      - Rebuild from scratch"
	@echo "  make run     - Build and run the game"
	@echo "  make debug   - Build and run with GDB"
	@echo "  make help    - Show this help message"
