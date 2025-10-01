cmake_minimum_required(VERSION 3.21)

# Check if vcpkg is available, if not set it up
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
        # Bootstrap vcpkg
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
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(R-Type)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

find_package(raylib CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

# Build ECS as a shared library
file(GLOB_RECURSE ECS_SOURCES "ECS/*.cpp")
if(ECS_SOURCES)
    add_library(ECS SHARED ${ECS_SOURCES})
    target_include_directories(ECS PUBLIC ECS ECS/include)
    target_link_libraries(ECS PUBLIC raylib::raylib)
endif()

# Build Game executable (r-type_client)
file(GLOB_RECURSE GAME_SOURCES "Game/*.cpp")
# Add Network protocol sources for the client
file(GLOB NETWORK_PROTOCOL_SOURCES "Network/src/protocol.cpp")
if(GAME_SOURCES)
    add_executable(r-type_client ${GAME_SOURCES} ${NETWORK_PROTOCOL_SOURCES})
    target_include_directories(r-type_client PRIVATE 
        Game
        Network/include
    )
    
    # Link with raylib and required dependencies
    target_link_libraries(r-type_client raylib)
    
    # Find and link GLFW explicitly
    target_link_libraries(r-type_client PRIVATE raylib::raylib)
    target_link_libraries(r-type_client glfw)

    # Link with ECS library if it exists
    if(TARGET ECS)
        target_link_libraries(r-type_client ECS)
    endif()
endif()

# Build Network executable (r-type_server)
file(GLOB_RECURSE NETWORK_SOURCES "Network/*.cpp")
if(NETWORK_SOURCES)
    add_executable(r-type_server ${NETWORK_SOURCES})
    target_include_directories(r-type_server PRIVATE 
        Network 
        Network/include
    )
    
    # Link with required system libraries for Asio
    find_package(Threads REQUIRED)
    target_link_libraries(r-type_server 
        Threads::Threads
        asio::asio
    )
    
    # Note: Server doesn't link with ECS to avoid raylib dependency
endif()

# Set output directories
set_target_properties(r-type_client r-type_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(TARGET ECS)
    set_target_properties(ECS PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

# Enable testing
enable_testing()

# Add test directories if they contain test files
file(GLOB ECS_TEST_SOURCES "ECS/test/*.cpp")
if(ECS_TEST_SOURCES)
    add_executable(test_ecs ${ECS_TEST_SOURCES})
    target_include_directories(test_ecs PRIVATE ECS)
    if(TARGET ECS)
        target_link_libraries(test_ecs ECS)
    endif()
    add_test(NAME ECS_Tests COMMAND test_ecs)
endif()

file(GLOB GAME_TEST_SOURCES "Game/test/*.cpp")
if(GAME_TEST_SOURCES)
    add_executable(test_game ${GAME_TEST_SOURCES})
    target_include_directories(test_game PRIVATE Game)
    if(TARGET ECS)
        target_link_libraries(test_game ECS)
    endif()
    add_test(NAME Game_Tests COMMAND test_game)
endif()

file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    add_executable(test_network ${NETWORK_TEST_SOURCES})
    target_include_directories(test_network PRIVATE Network)
    add_test(NAME Network_Tests COMMAND test_network)
endif()