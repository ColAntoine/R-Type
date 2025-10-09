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
find_package(glfw3 CONFIG REQUIRED)

# Build ECS as a shared library (EXCLUDE system files)
file(GLOB_RECURSE ECS_SOURCES "ECS/*.cpp")
file(GLOB_RECURSE SYSTEM_SOURCES "ECS/src/systems/*.cpp")
list(REMOVE_ITEM ECS_SOURCES ${SYSTEM_SOURCES})

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

# Add subdirectories for modular builds
add_subdirectory(Games/RType)

# Build Game executable (r-type_client)
# Check if main file exists in Games directory (for backwards compatibility)
set(MAIN_SOURCE "")
if(EXISTS "${CMAKE_SOURCE_DIR}/Games/main.cpp")
    set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/Games/main.cpp")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/Games/RType/main.cpp")
    set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/Games/RType/main.cpp")
endif()

# Add Network protocol sources for the client
file(GLOB NETWORK_PROTOCOL_SOURCES "Network/src/protocol.cpp")

if(MAIN_SOURCE)
    add_executable(r-type_client ${MAIN_SOURCE} ${NETWORK_PROTOCOL_SOURCES})
    
    # Link with RType library
    target_link_libraries(r-type_client PRIVATE
        RType
        raylib
        glfw
        dl
    )
    
    target_include_directories(r-type_client PRIVATE
        Games/RType
        Network/include
        ${RTYPE_INCLUDE_DIRS}
    )

    # Link with ECS library if it exists
    if(TARGET ECS)
        target_link_libraries(r-type_client PRIVATE ECS)
    endif()
else()
    message(WARNING "No main.cpp found for r-type_client")
endif()

# ============================================================================
# Build r-type_server Executable
# ============================================================================
# ✅ NOUVEAU: Utiliser Games/RType/Core/Server/server_main.cpp comme point d'entrée
if(EXISTS "${CMAKE_SOURCE_DIR}/Games/RType/Core/Server/server_main.cpp")
    # Collecter UNIQUEMENT les sources Network (SANS main.cpp qui n'existe plus)
    file(GLOB_RECURSE NETWORK_SOURCES "Network/src/*.cpp")
    file(GLOB_RECURSE NETWORK_INCLUDE "Network/include/*.hpp")
    
    # Exclure les tests
    list(FILTER NETWORK_SOURCES EXCLUDE REGEX ".*/Network/test/.*")
    
    # ✅ Créer l'exécutable avec server_main.cpp + Network sources
    add_executable(r-type_server 
        "${CMAKE_SOURCE_DIR}/Games/RType/Core/Server/server_main.cpp"
        ${NETWORK_SOURCES}
    )

    target_include_directories(r-type_server PRIVATE
        Network
        Network/include
        Games/RType
        Games/RType/Core
        Games/RType/Core/Server
        ECS/include
    )

    # Link avec les dépendances nécessaires
    find_package(Threads REQUIRED)
    target_link_libraries(r-type_server PRIVATE
        RType
        Threads::Threads
        asio::asio
    )

    # ✅ Link avec ECS si le serveur en a besoin
    if(TARGET ECS)
        target_link_libraries(r-type_server PRIVATE ECS)
    endif()

    message(STATUS "✅ r-type_server configured with Games/RType/Core/Server/server_main.cpp")
else()
    message(FATAL_ERROR "❌ Games/RType/Core/Server/server_main.cpp NOT FOUND!")
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

file(GLOB GAME_TEST_SOURCES "Games/test/*.cpp")
if(GAME_TEST_SOURCES)
    # Add Network protocol sources for the test
    file(GLOB NETWORK_PROTOCOL_SOURCES "Network/src/protocol.cpp")

    add_executable(test_game ${GAME_TEST_SOURCES} ${NETWORK_PROTOCOL_SOURCES})
    
    # Link with RType library and its dependencies
    target_link_libraries(test_game PRIVATE
        RType
        raylib
        glfw
        dl
    )
    
    target_include_directories(test_game PRIVATE
        Games/RType
        Network/include
        ${RTYPE_INCLUDE_DIRS}
    )
    
    if(TARGET ECS)
        target_link_libraries(test_game PRIVATE ECS)
    endif()
    
    add_test(NAME Game_Tests COMMAND test_game)
endif()

file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    # Add necessary Network source files for testing
    file(GLOB_RECURSE NETWORK_IMPL_SOURCES "Network/src/*.cpp")
    list(FILTER NETWORK_IMPL_SOURCES EXCLUDE REGEX ".*/Network/test/.*")

    add_executable(test_network ${NETWORK_TEST_SOURCES} ${NETWORK_IMPL_SOURCES})
    target_include_directories(test_network PRIVATE Network Network/include)

    # Find and link doctest
    find_package(doctest CONFIG REQUIRED)
    target_link_libraries(test_network doctest::doctest)

    # Link with required system libraries for Asio (same as server)
    find_package(Threads REQUIRED)
    target_link_libraries(test_network Threads::Threads asio::asio)

    add_test(NAME Network_Tests COMMAND test_network)
endif()