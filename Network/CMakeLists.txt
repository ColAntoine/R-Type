cmake_minimum_required(VERSION 3.16)
project(RType_Network VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Find packages
find_package(Threads REQUIRED)

# Platform detection
if(WIN32)
    set(IS_WINDOWS TRUE)
else()
    set(IS_WINDOWS FALSE)
endif()

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

# Include directories
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/ECS/include)


if(USE_STANDALONE_ASIO)
    include_directories(${ASIO_INCLUDE_DIR})
    add_definitions(-DASIO_STANDALONE)
    if(IS_WINDOWS)
        add_definitions(-D_WIN32_WINNT=0x0601)
    endif()
endif()

# Source files
set(NETWORK_SOURCES
    src/network_manager.cpp
    src/connection.cpp
    src/session.cpp
    src/udp_server.cpp
    src/message_handler.cpp
    src/protocol.cpp
    src/game_handlers.cpp
)

# Header files (for IDE support)
set(NETWORK_HEADERS
    include/network_manager.hpp
    include/connection.hpp
    include/session.hpp
    include/udp_server.hpp
    include/message_handler.hpp
    include/protocol.hpp
    include/game_handlers.hpp
)

# Create the network library
add_library(rtype_network STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

# Set target properties
target_include_directories(rtype_network PUBLIC include)
# Allow network code to include ECS headers when linking against ECS
if(TARGET ECS)
    target_include_directories(rtype_network PRIVATE ${CMAKE_SOURCE_DIR}/ECS/include)
endif()

if(USE_STANDALONE_ASIO)
    target_include_directories(rtype_network PUBLIC ${ASIO_INCLUDE_DIR})
    target_compile_definitions(rtype_network PUBLIC ASIO_STANDALONE)
endif()

# Link libraries
target_link_libraries(rtype_network PUBLIC Threads::Threads)


if(USE_BOOST_ASIO)
    target_link_libraries(rtype_network PUBLIC Boost::system)
endif()

# Link ws2_32 on Windows
if(IS_WINDOWS)
    target_link_libraries(rtype_network PUBLIC ws2_32)
endif()

# Create the server executable
add_executable(rtype_server main.cpp)
target_link_libraries(rtype_server PRIVATE rtype_network)
if(TARGET ECS)
    target_link_libraries(rtype_server PRIVATE ECS)
    target_include_directories(rtype_server PRIVATE ${CMAKE_SOURCE_DIR}/ECS ${CMAKE_SOURCE_DIR}/ECS/include)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(rtype_network PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(rtype_server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable debug symbols in debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Installation (optional)
install(TARGETS rtype_server rtype_network
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/rtype_network)

# Print configuration summary
message(STATUS "=== RType Network Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(USE_STANDALONE_ASIO)
    message(STATUS "Asio: Standalone (${ASIO_INCLUDE_DIR})")
else()
    message(STATUS "Asio: Boost.Asio ${Boost_VERSION}")
endif()
message(STATUS "====================================")