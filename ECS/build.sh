#!/bin/bash

COVERAGE=OFF
AUDIO_TESTS=OFF

while [[ $# -gt 0 ]]; do
    case $1 in
        --coverage|-c) COVERAGE=ON; shift ;;
        --audio-tests|-a) AUDIO_TESTS=ON; shift ;;
        --help|-h)
            echo "Usage: ./build.sh [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --coverage, -c       Enable code coverage"
            echo "  --audio-tests, -a    Enable audio tests (requires PulseAudio)"
            echo "  --help, -h           Show this help"
            exit 0 ;;
        *)
            echo "Unknown option: $1"
            exit 1 ;;
    esac
done

mkdir -p build && cd build

CMAKE_OPTS=""
[ "$COVERAGE" = "ON" ] && CMAKE_OPTS="$CMAKE_OPTS -DENABLE_COVERAGE=ON" && echo "🔍 Coverage enabled"
[ "$AUDIO_TESTS" = "ON" ] && CMAKE_OPTS="$CMAKE_OPTS -DENABLE_AUDIO_TESTS=ON" && echo "🔊 Audio tests enabled"

[ -z "$CMAKE_OPTS" ] && echo "🔨 Building..." || echo "🔨 Building with options..."

cmake $CMAKE_OPTS .. && make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    [ "$COVERAGE" = "ON" ] && echo "Run: cd build && ctest && lcov --capture -d . -o coverage.info"
else
    echo "❌ Build failed!"
    exit 1
fi
