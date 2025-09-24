#!/bin/bash

set -e

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

cmake ..

gmake

mv ecs ../
mv libcomponents.so ../
cd ..

# Print success message
echo "Build completed successfully!"