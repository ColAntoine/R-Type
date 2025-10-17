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

# Add ECS subdirectory to use its proper CMakeLists.txt
add_subdirectory(ECS)

add_custom_target(copy_libs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/bin/lib
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
        ECS
        raylib
        glfw
        dl
    )

    target_include_directories(r-type_client PRIVATE
        Games/RType
        Network/include
        ECS/include
        ${RTYPE_INCLUDE_DIRS}
    )
    
    set_target_properties(r-type_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
else()
    message(WARNING "No main.cpp found for r-type_client")
endif()

# Build Network executable (r-type_server)
file(GLOB_RECURSE NETWORK_SOURCES "Network/*.cpp")
list(FILTER NETWORK_SOURCES EXCLUDE REGEX ".*/Network/test/.*")
if(NETWORK_SOURCES)
    add_executable(r-type_server ${NETWORK_SOURCES})
    target_include_directories(r-type_server PRIVATE
        Network
        Network/include
    )

    # Link with required system libraries for Asio
    find_package(Threads REQUIRED)
    target_link_libraries(r-type_server PRIVATE
        Threads::Threads
        asio::asio
    )

    # Ensure server links with the RType game library so component symbols (Enemy, drawable, Health, etc.) are available
    if(TARGET RType)
        target_link_libraries(r-type_server PRIVATE RType)
        target_include_directories(r-type_server PRIVATE ${CMAKE_SOURCE_DIR}/Games/RType)
    endif()

    # Allow server to use ECS types (registry, component factory) when ECS target exists
    if(TARGET ECS)
        target_link_libraries(r-type_server PRIVATE ECS)
        target_include_directories(r-type_server PRIVATE ${CMAKE_SOURCE_DIR}/ECS/include)
    endif()
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
    target_include_directories(test_ecs PRIVATE 
        ${CMAKE_SOURCE_DIR}/ECS/include
        ${CMAKE_SOURCE_DIR}/ECS
    )
    if(TARGET ECS)
        target_link_libraries(test_ecs ECS raylib)
    endif()
    
    # Find and link doctest
    find_package(doctest CONFIG REQUIRED)
    target_link_libraries(test_ecs doctest::doctest)
    
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
    # Add necessary Network source files for testing (excluding main)
    file(GLOB_RECURSE NETWORK_IMPL_SOURCES "Network/*.cpp")
    list(FILTER NETWORK_IMPL_SOURCES EXCLUDE REGEX ".*/Network/test/.*")
    list(FILTER NETWORK_IMPL_SOURCES EXCLUDE REGEX ".*/Network/main.cpp")

    add_executable(test_network ${NETWORK_TEST_SOURCES} ${NETWORK_IMPL_SOURCES})
    target_include_directories(test_network PRIVATE Network Network/include)
    if(TARGET ECS)
        target_include_directories(test_network PRIVATE ${CMAKE_SOURCE_DIR}/ECS/include)
        target_link_libraries(test_network PRIVATE ECS)
    endif()

    # Ensure test_network links with RType to provide component implementations for tests
    if(TARGET RType)
        target_link_libraries(test_network PRIVATE RType)
        target_include_directories(test_network PRIVATE ${CMAKE_SOURCE_DIR}/Games/RType)
    endif()

    # Find and link doctest
    find_package(doctest CONFIG REQUIRED)
    target_link_libraries(test_network PRIVATE doctest::doctest)

    # Link with required system libraries for Asio (same as server)
    find_package(Threads REQUIRED)
    target_link_libraries(test_network PRIVATE Threads::Threads asio::asio)

    add_test(NAME Network_Tests COMMAND test_network)
endif()