#!/bin/bash

set -e

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Get the project root (parent of scripts directory)
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "Project root: $PROJECT_ROOT"
cd "$PROJECT_ROOT"

mkdir -p external

if [ ! -d "external/vcpkg" ]; then
    echo "Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git external/vcpkg
fi

if [ ! -f "external/vcpkg/vcpkg" ]; then
    echo "Bootstrapping vcpkg..."
    cd external/vcpkg
    ./bootstrap-vcpkg.sh
    cd "$PROJECT_ROOT"
fi

echo "Installing vcpkg packages..."
./external/vcpkg/vcpkg install

echo "Build script completed successfully!"