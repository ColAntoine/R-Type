cmake_minimum_required(VERSION 3.21)
project(RType_Network VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Find Dependencies
# ============================================================================
find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

# ============================================================================
# Collect Source Files for Server
# ============================================================================
file(GLOB_RECURSE NETWORK_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Filter out test files
list(FILTER NETWORK_SOURCES EXCLUDE REGEX ".*/test/.*")

# ============================================================================
# Build r-type_server Executable
# ============================================================================
add_executable(r-type_server ${NETWORK_SOURCES})

# Include directories
target_include_directories(r-type_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(r-type_server PRIVATE
    Threads::Threads
    asio::asio
)

# Set output directory
set_target_properties(r-type_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Compiler flags
target_compile_options(r-type_server PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

message(STATUS "Building r-type_server with sources: ${NETWORK_SOURCES}")
