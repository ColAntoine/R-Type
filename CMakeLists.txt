cmake_minimum_required(VERSION 3.21)

# ------------------------------------------------------------------
# Automatically initialize vcpkg if no toolchain is provided
# ------------------------------------------------------------------
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/external/vcpkg")
    if(NOT EXISTS "${VCPKG_ROOT}")
        find_package(Git REQUIRED)
        message(STATUS "Cloning vcpkg...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT}
            RESULT_VARIABLE git_result
        )
        if(NOT git_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone vcpkg")
        endif()

        # Bootstrap vcpkg (Windows or Unix)
        if(WIN32)
            execute_process(
                COMMAND ${VCPKG_ROOT}/bootstrap-vcpkg.bat
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE bootstrap_result
            )
        else()
            execute_process(
                COMMAND ${VCPKG_ROOT}/bootstrap-vcpkg.sh
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE bootstrap_result
            )
        endif()

        if(NOT bootstrap_result EQUAL 0)
            message(FATAL_ERROR "Failed to bootstrap vcpkg")
        endif()
    endif()

    # Set the toolchain file so CMake uses vcpkg
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(R-Type)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------
# Fun ASCII art 🎨
# ------------------------------------------------------------------
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

# ------------------------------------------------------------------
# Dependencies declared through vcpkg (or system)
# ------------------------------------------------------------------
find_package(raylib CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Unit tests (doctest)
find_package(doctest CONFIG REQUIRED)
include(doctest)
include(CTest)

# ------------------------------------------------------------------
# ECS: Shared library
# ------------------------------------------------------------------
file(GLOB_RECURSE ECS_SOURCES "ECS/*.cpp")
file(GLOB_RECURSE SYSTEM_SOURCES "ECS/src/systems/*.cpp")
list(REMOVE_ITEM ECS_SOURCES ${SYSTEM_SOURCES})
# Exclude test files and main.cpp
list(FILTER ECS_SOURCES EXCLUDE REGEX ".*/ECS/test/.*")
list(FILTER ECS_SOURCES EXCLUDE REGEX ".*/ECS/main\\.cpp$")

if(ECS_SOURCES)
    add_library(ECS SHARED ${ECS_SOURCES})
    target_include_directories(ECS PUBLIC ECS ECS/include)
    target_link_libraries(ECS raylib glfw dl)
endif()

# Build individual system libraries
file(GLOB SYSTEM_FILES "ECS/src/systems/*.cpp")
foreach(SYSTEM_FILE ${SYSTEM_FILES})
    get_filename_component(SYSTEM_NAME ${SYSTEM_FILE} NAME_WE)
    add_library(${SYSTEM_NAME} SHARED ${SYSTEM_FILE})
    target_include_directories(${SYSTEM_NAME} PUBLIC ECS ECS/include)
    target_link_libraries(${SYSTEM_NAME} ECS raylib glfw)

    # Set output directory for system libraries
    set_target_properties(${SYSTEM_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems
    )
endforeach()

add_custom_target(copy_libs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/bin/lib
    DEPENDS ${SYSTEM_FILES}
    COMMENT "Copying system libraries to binary directory"
)

if(TARGET r-type_client)
    add_dependencies(r-type_client copy_libs)

endif()

# ------------------------------------------------------------------
# Client executable (r-type_client)
# ------------------------------------------------------------------
file(GLOB_RECURSE GAME_SOURCES "Game/*.cpp")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/Game/test/.*")

file(GLOB NETWORK_PROTOCOL_SOURCES "Network/src/protocol.cpp")

if(GAME_SOURCES)
    add_executable(r-type_client ${GAME_SOURCES} ${NETWORK_PROTOCOL_SOURCES})
    target_include_directories(r-type_client PRIVATE
        Game
        Network/include
    )
    
    # Link with raylib and required dependencies
    target_link_libraries(r-type_client raylib glfw dl)

    # Link with ECS library if it exists
    if(TARGET ECS)
        # No need to link raylib+glfw twice: already linked in ECS
        target_link_libraries(r-type_client PRIVATE ECS)
    else()
        target_link_libraries(r-type_client PRIVATE raylib glfw)
    endif()
endif()

# ------------------------------------------------------------------
# Server executable (r-type_server)
# ------------------------------------------------------------------
file(GLOB_RECURSE NETWORK_SOURCES "Network/*.cpp")
list(FILTER NETWORK_SOURCES EXCLUDE REGEX ".*/Network/test/.*")

if(NETWORK_SOURCES)
    add_executable(r-type_server ${NETWORK_SOURCES})
    target_include_directories(r-type_server PRIVATE
        Network
        Network/include
    )
    target_link_libraries(r-type_server
        PRIVATE
            Threads::Threads
            asio::asio
    )
endif()

# ------------------------------------------------------------------
# Output directories for binaries and libraries
# ------------------------------------------------------------------
foreach(target r-type_client r-type_server ECS)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        )
    endif()
endforeach()

# ------------------------------------------------------------------
# Unit test configuration
# ------------------------------------------------------------------
enable_testing()

# ------------------------
# ECS tests
# ------------------------
file(GLOB ECS_TEST_SOURCES "ECS/test/*.cpp")
if(ECS_TEST_SOURCES)
    add_executable(test_ecs ${ECS_TEST_SOURCES})
    target_include_directories(test_ecs PRIVATE ECS ECS/include)
    if(TARGET ECS)
        target_link_libraries(test_ecs PRIVATE ECS)
    endif()
    # Choose one of the following lines depending on your doctest config
    target_link_libraries(test_ecs PRIVATE doctest::doctest)
    # target_link_libraries(test_ecs PRIVATE doctest::doctest_with_main)
    doctest_discover_tests(test_ecs)
endif()

# ------------------------
# Game tests
# ------------------------
file(GLOB GAME_TEST_SOURCES "Game/test/*.cpp")
if(GAME_TEST_SOURCES)
    add_executable(test_game ${GAME_TEST_SOURCES})
    target_include_directories(test_game PRIVATE Game)

    # Add GameWindow implementation for tests
    target_sources(test_game PRIVATE Game/game_window.cpp)

    if(TARGET ECS)
        target_link_libraries(test_game PRIVATE ECS)
    endif()

    # Tests call raylib through GameWindow
    target_link_libraries(test_game PRIVATE raylib glfw)

    # Assume main is provided in the tests via DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
    target_link_libraries(test_game PRIVATE doctest::doctest)
    doctest_discover_tests(test_game)
endif()

# ------------------------
# Network tests
# ------------------------
file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    add_executable(test_network ${NETWORK_TEST_SOURCES})
    target_include_directories(test_network PRIVATE Network Network/include)

    # Add required source files for network tests
    target_sources(test_network PRIVATE
        Network/src/connection.cpp
    )

    # Link Asio and doctest
    target_link_libraries(test_network
        PRIVATE
            asio::asio
            Threads::Threads
            doctest::doctest
    )

    # Uncomment this section if some network tests depend on ECS:
    # if(TARGET ECS)
    #     target_link_libraries(test_network PRIVATE ECS)
    # endif()

    doctest_discover_tests(test_network)
endif()
