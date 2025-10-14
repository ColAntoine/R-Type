#!/bin/bash

set -e

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

cmake ..

gmake

# Copy libraries to parent directory
cp -r lib ../
cp libECS.so ../lib
cd ..

echo ""
echo "✓ Build successful!"
echo "Generated libraries:"
echo "  - libECS.so (Core ECS library)"
echo "  - lib/systems/libposition_system.so"
echo "  - lib/systems/libcollision_system.so"
echo "  - lib/systems/libsprite_system.so"
echo "  - lib/systems/libanimation_system.so"

# Print success message
echo "Build completed successfully!"