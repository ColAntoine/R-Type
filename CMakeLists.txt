cmake_minimum_required(VERSION 3.21)

# ------------------------------------------------------------------
# vcpkg bootstrap si aucun toolchain n'est fourni à CMake
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

# Petit ASCII art :)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

# ------------------------------------------------------------------
# Dépendances via vcpkg (ou système)
# ------------------------------------------------------------------
find_package(raylib CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Tests (doctest)
find_package(doctest CONFIG REQUIRED)
include(doctest)
include(CTest)

# ------------------------------------------------------------------
# ECS : bibliothèque partagée
# ------------------------------------------------------------------
file(GLOB_RECURSE ECS_SOURCES "ECS/*.cpp")
# Exclure les fichiers de test et le main
list(FILTER ECS_SOURCES EXCLUDE REGEX ".*/ECS/test/.*")
list(FILTER ECS_SOURCES EXCLUDE REGEX ".*/ECS/main\\.cpp$")

if(ECS_SOURCES)
    add_library(ECS SHARED ${ECS_SOURCES})
    target_include_directories(ECS PUBLIC ECS ECS/include)
    # ECS appelle des fonctions raylib/glfw => on exporte les dépendances
    target_link_libraries(ECS PUBLIC raylib glfw)
endif()

# ------------------------------------------------------------------
# Client (r-type_client)
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
    if(TARGET ECS)
        # ECS exporte déjà raylib+glfw → inutile de les relier deux fois
        target_link_libraries(r-type_client PRIVATE ECS)
    else()
        target_link_libraries(r-type_client PRIVATE raylib glfw)
    endif()
endif()

# ------------------------------------------------------------------
# Serveur (r-type_server)
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
# Dossiers de sortie
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
# Tests
# ------------------------------------------------------------------
enable_testing()

# ECS tests
file(GLOB ECS_TEST_SOURCES "ECS/test/*.cpp")
if(ECS_TEST_SOURCES)
    add_executable(test_ecs ${ECS_TEST_SOURCES})
    target_include_directories(test_ecs PRIVATE ECS ECS/include)
    if(TARGET ECS)
        target_link_libraries(test_ecs PRIVATE ECS)
    endif()
    # ⚠️ Choisis UNE des deux selon ta config doctest
    target_link_libraries(test_ecs PRIVATE doctest::doctest)
    # target_link_libraries(test_ecs PRIVATE doctest::doctest_with_main)
    doctest_discover_tests(test_ecs)
endif()

# Game tests
file(GLOB GAME_TEST_SOURCES "Game/test/*.cpp")
if(GAME_TEST_SOURCES)
    add_executable(test_game ${GAME_TEST_SOURCES})
    target_include_directories(test_game PRIVATE Game)

    # ⬇️ AJOUTE CETTE LIGNE pour compiler l'implémentation de GameWindow dans le test
    target_sources(test_game PRIVATE Game/game_window.cpp)

    if(TARGET ECS)
        target_link_libraries(test_game PRIVATE ECS)
    endif()

    # le test appelle raylib via GameWindow
    target_link_libraries(test_game PRIVATE raylib glfw)

    # un seul main doctest (fourni par le test via DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
    target_link_libraries(test_game PRIVATE doctest::doctest)
    doctest_discover_tests(test_game)
endif()

# Network tests
file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    add_executable(test_network ${NETWORK_TEST_SOURCES})
    target_include_directories(test_network PRIVATE Network Network/include)

    # 👇 Ajouter l'implémentation testée
    target_sources(test_network PRIVATE
        Network/src/connection.cpp
    )

    # Liens nécessaires pour Asio
    target_link_libraries(test_network
        PRIVATE
            asio::asio
            Threads::Threads
            doctest::doctest
    )

    # Si certains tests réseau utilisent ECS, décommente :
    # if(TARGET ECS)
    #     target_link_libraries(test_network PRIVATE ECS)
    # endif()

    doctest_discover_tests(test_network)
endif()
