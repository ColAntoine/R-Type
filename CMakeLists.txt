cmake_minimum_required(VERSION 3.21)

# -------------------------------------------------------------------
# Toolchain vcpkg (auto-clone/bootstrappé si absent)
# -------------------------------------------------------------------
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

# -------------------------------------------------------------------
# Projet / langage
# -------------------------------------------------------------------
project(R-Type)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Petit ASCII art
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

# -------------------------------------------------------------------
# Dépendances via vcpkg
# -------------------------------------------------------------------
# raylib / glfw / asio (targets importées)
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)      # fournit la target 'glfw'
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)

# -------------------------------------------------------------------
# ECS (lib partagée)
# -------------------------------------------------------------------
file(GLOB_RECURSE ECS_SOURCES "ECS/*.cpp")
if(ECS_SOURCES)
    add_library(ECS SHARED ${ECS_SOURCES})
    target_include_directories(ECS PUBLIC
        ECS
        ECS/include
    )
    # ECS inclut <raylib.h> d'après tes sources → on lie PUBLIC pour propager includes/defs
    target_link_libraries(ECS PUBLIC raylib::raylib)
    # Si un jour ECS utilise aussi GLFW, décommente :
    # target_link_libraries(ECS PUBLIC glfw)
endif()

# -------------------------------------------------------------------
# Client (r-type_client)
# -------------------------------------------------------------------
file(GLOB_RECURSE GAME_SOURCES "Game/*.cpp")
# Ajout du protocole réseau côté client
file(GLOB NETWORK_PROTOCOL_SOURCES "Network/src/protocol.cpp")

if(GAME_SOURCES)
    add_executable(r-type_client
        ${GAME_SOURCES}
        ${NETWORK_PROTOCOL_SOURCES}
    )
    target_include_directories(r-type_client PRIVATE
        Game
        Network/include
    )
    # Liaisons (signature KEYWORD uniquement)
    target_link_libraries(r-type_client PRIVATE
        raylib::raylib
        glfw
    )
    if(TARGET ECS)
        target_link_libraries(r-type_client PRIVATE ECS)
    endif()
endif()

# -------------------------------------------------------------------
# Serveur (r-type_server)
# -------------------------------------------------------------------
file(GLOB_RECURSE NETWORK_SOURCES "Network/*.cpp")
if(NETWORK_SOURCES)
    add_executable(r-type_server ${NETWORK_SOURCES})
    target_include_directories(r-type_server PRIVATE
        Network
        Network/include
    )
    target_link_libraries(r-type_server PRIVATE
        Threads::Threads
        asio::asio
    )
    # Note : pas de lien avec ECS pour éviter la dépendance raylib sur le serveur
endif()

# -------------------------------------------------------------------
# Répertoires de sortie
# -------------------------------------------------------------------
if(TARGET r-type_client)
    set_target_properties(r-type_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

if(TARGET r-type_server)
    set_target_properties(r-type_server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

if(TARGET ECS)
    set_target_properties(ECS PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

# -------------------------------------------------------------------
# Tests (ctest)
# -------------------------------------------------------------------
enable_testing()

file(GLOB ECS_TEST_SOURCES "ECS/test/*.cpp")
if(ECS_TEST_SOURCES)
    add_executable(test_ecs ${ECS_TEST_SOURCES})
    target_include_directories(test_ecs PRIVATE ECS)
    if(TARGET ECS)
        target_link_libraries(test_ecs PRIVATE ECS)
    endif()
    add_test(NAME ECS_Tests COMMAND test_ecs)
endif()

file(GLOB GAME_TEST_SOURCES "Game/test/*.cpp")
if(GAME_TEST_SOURCES)
    add_executable(test_game ${GAME_TEST_SOURCES})
    target_include_directories(test_game PRIVATE Game)
    if(TARGET ECS)
        target_link_libraries(test_game PRIVATE ECS)
    endif()
    add_test(NAME Game_Tests COMMAND test_game)
endif()

file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    add_executable(test_network ${NETWORK_TEST_SOURCES})
    target_include_directories(test_network PRIVATE Network)
    target_link_libraries(test_network PRIVATE asio::asio Threads::Threads)
    add_test(NAME Network_Tests COMMAND test_network)
endif()
