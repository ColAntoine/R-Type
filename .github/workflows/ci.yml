name: CI - Build & Test

on:
  pull_request:
    branches: [ main ]
  push:
    branches-ignore: [ main ]

concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    env:
      CMAKE_BUILD_TYPE: Release
      CTEST_OUTPUT_ON_FAILURE: 1
      CMAKE_BUILD_PARALLEL_LEVEL: 3
      VCPKG_ROOT: ${{ github.workspace }}/external/vcpkg
      VCPKG_TOOLCHAIN: ${{ github.workspace }}/external/vcpkg/scripts/buildsystems/vcpkg.cmake

    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Cache vcpkg binary archives
        uses: actions/cache@v4
        with:
          path: ~/.cache/vcpkg/archives
          key: v1-${{ runner.os }}-vcpkg-${{ hashFiles('**/CMakeLists.txt', '**/vcpkg*.{json,txt}') }}
          restore-keys: |
            v1-${{ runner.os }}-vcpkg-

      - name: Install system deps (OpenGL/X11 + headless)
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            git build-essential cmake ninja-build pkg-config \
            xorg-dev libglu1-mesa-dev xvfb \
            libasound2-dev libpulse-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev libudev-dev

      - name: Prepare vcpkg (clone + bootstrap if needed)
        shell: bash
        run: |
          set -euxo pipefail
          if [ ! -f "${VCPKG_TOOLCHAIN}" ]; then
            rm -rf "${VCPKG_ROOT}"
            git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}"
            "${VCPKG_ROOT}/bootstrap-vcpkg.sh" -disableMetrics
          fi
          # Sanity check
          test -f "${VCPKG_TOOLCHAIN}"

      - name: Setup ccache
        uses: hendrikmuhs/ccache-action@v1
        with:
          key: rtype-${{ runner.os }}

      - name: Configure (CMake + Ninja)
        shell: bash
        run: |
          cmake -S . -B build -G Ninja \
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
            -DCMAKE_TOOLCHAIN_FILE="${VCPKG_TOOLCHAIN}" \
            -DCMAKE_C_COMPILER_LAUNCHER=ccache \
            -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
            -DOpenGL_GL_PREFERENCE=GLVND

      - name: Dump configure context on failure
        if: failure()
        shell: bash
        run: |
          echo "::group::Tree build/"
          ls -la build || true
          echo "::endgroup::"
          echo "::group::CMakeFiles logs"
          [ -f build/CMakeFiles/CMakeOutput.log ] && tail -n +1 -v build/CMakeFiles/CMakeOutput.log || true
          [ -f build/CMakeFiles/CMakeError.log ] && tail -n +1 -v build/CMakeFiles/CMakeError.log || true
          echo "::endgroup::"

      - name: Build
        run: cmake --build build --config "${CMAKE_BUILD_TYPE}"

      - name: Run tests (ctest under Xvfb)
        run: |
          xvfb-run -a -s "-screen 0 1920x1080x24" \
            ctest --test-dir build --output-on-failure -C "${CMAKE_BUILD_TYPE}"

      - name: Upload build/test logs on failure
        if: failure()
        uses: actions/upload-artifact@v4
        with:
          name: build-logs
          path: |
            build/Testing
            build/CMakeFiles/CMakeOutput.log
            build/CMakeFiles/CMakeError.log
