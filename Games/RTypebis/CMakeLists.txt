cmake_minimum_required(VERSION 3.21)
project(RTypebis LANGUAGES CXX)

# Use C++20 (adjust if you need a different standard)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Collect all .cpp files under this directory
file(GLOB_RECURSE RTB_SOURCES "${SOURCE_DIR}/*.cpp")

# Exclude CMake/build generated files (prevents picking up CMakeCXXCompilerId.cpp when doing in-source build)
list(FILTER RTB_SOURCES EXCLUDE REGEX ".*/CMakeFiles/.*")
list(FILTER RTB_SOURCES EXCLUDE REGEX ".*/build/.*")
list(FILTER RTB_SOURCES EXCLUDE REGEX ".*/CMakeCXXCompilerId\\.cpp$")

# Exclude any files under Server/ and the main_server.cpp file
list(FILTER RTB_SOURCES EXCLUDE REGEX ".*/Server/.*")
list(FILTER RTB_SOURCES EXCLUDE REGEX ".*/main_server.cpp$")

if(NOT RTB_SOURCES)
    message(FATAL_ERROR "No source files found in ${SOURCE_DIR} after filtering.")
endif()

add_executable(r-type_client ${RTB_SOURCES})

# Expose the current directory as an include dir (add more if needed)
target_include_directories(r-type_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ECS/include/
)

# Link WinSock on Windows
if (WIN32)
    target_link_libraries(r-type_client PRIVATE ws2_32)
endif()

# Place the built executable in build/bin
set_target_properties(r-type_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Command to compile
# mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . --config Release -- -j$(nproc)