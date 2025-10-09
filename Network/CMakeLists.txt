cmake_minimum_required(VERSION 3.21)
project(RType_Network VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ============================================================================
# Find Dependencies
# ============================================================================
find_package(Threads REQUIRED)

# Try to find Asio (standalone or Boost.Asio)
find_path(ASIO_INCLUDE_DIR 
    NAMES asio.hpp
    PATHS /usr/include /usr/local/include /opt/homebrew/include
    PATH_SUFFIXES asio standalone_asio
)

if(NOT ASIO_INCLUDE_DIR)
    # Fallback to Boost.Asio
    find_package(Boost REQUIRED COMPONENTS system)
    if(Boost_FOUND)
        set(USE_BOOST_ASIO ON)
        message(STATUS "Using Boost.Asio")
    else()
        message(FATAL_ERROR "Neither standalone Asio nor Boost.Asio found")
    endif()
else()
    set(USE_STANDALONE_ASIO ON)
    message(STATUS "Using standalone Asio from: ${ASIO_INCLUDE_DIR}")
endif()

# ============================================================================
# Source Files (Transport Layer only)
# ============================================================================
set(NETWORK_SOURCES
    src/network_manager.cpp
    src/connection.cpp
    src/session.cpp
    src/udp_server.cpp
    src/message_handler.cpp
    src/protocol.cpp
    src/game_handlers.cpp
)

set(NETWORK_HEADERS
    include/network_manager.hpp
    include/connection.hpp
    include/session.hpp
    include/udp_server.hpp
    include/message_handler.hpp
    include/protocol.hpp
    include/game_handlers.hpp
)

# ============================================================================
# Network Library (Transport Layer)
# ============================================================================
add_library(rtype_network STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

set_target_properties(rtype_network PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rtype_network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(USE_STANDALONE_ASIO)
    target_include_directories(rtype_network PUBLIC ${ASIO_INCLUDE_DIR})
    target_compile_definitions(rtype_network PUBLIC ASIO_STANDALONE)
endif()

# Link libraries
target_link_libraries(rtype_network PUBLIC Threads::Threads)

if(USE_BOOST_ASIO)
    target_link_libraries(rtype_network PUBLIC Boost::system)
endif()

# ============================================================================
# Compiler Flags
# ============================================================================
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(rtype_network PRIVATE -Wall -Wextra -Wpedantic -fPIC)
endif()

# Enable debug symbols in debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# ============================================================================
# Installation (optional)
# ============================================================================
install(TARGETS rtype_network
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/rtype_network)

# ============================================================================
# Print Configuration Summary
# ============================================================================
message(STATUS "=== RType Network Configuration ===")
message(STATUS "  Transport Layer Only (no game logic)")
message(STATUS "  ASIO: ${USE_STANDALONE_ASIO}")
message(STATUS "  Boost.ASIO: ${USE_BOOST_ASIO}")
message(STATUS "  Sources: ${NETWORK_SOURCES}")
message(STATUS "===================================")
