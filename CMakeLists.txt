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

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

find_package(raylib CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(ECS)
add_subdirectory(Games/RType)

# Build Game executable (r-type_client)
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

    find_package(Threads REQUIRED)
    target_link_libraries(r-type_server
        Threads::Threads
        asio::asio
    )
endif()

# Set output directories
set_target_properties(r-type_client r-type_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy all system libraries - this needs to depend on ALL system targets
add_custom_target(copy_libs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    DEPENDS
        ECS
        RType
        # ECS Systems
        collision_system
        position_system
        sprite_system
        # Game Systems
        game_Control
        game_Draw
        game_EnemyAI
        game_EnemyCleanup
        game_EnemySpawnSystem
        game_LifeTime
        game_NetworkSyncSystem
        game_Spawn
    COMMENT "Copying libraries (waiting for all systems to be built)"
)

# Copy build/lib to project root AFTER all systems are built
add_custom_command(TARGET copy_libs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib ${CMAKE_SOURCE_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E echo "All systems built, copying lib directory to project root"
    COMMENT "Copying lib directory to project root"
)

add_dependencies(r-type_client copy_libs)

# Enable testing
enable_testing()

# Add test executables
file(GLOB ECS_TEST_SOURCES "ECS/test/*.cpp")
if(ECS_TEST_SOURCES)
    add_executable(test_ecs ${ECS_TEST_SOURCES})
    target_include_directories(test_ecs PRIVATE ECS/include)
    target_link_libraries(test_ecs ECS)
    add_test(NAME ECS_Tests COMMAND test_ecs)
endif()

file(GLOB GAME_TEST_SOURCES "Games/test/*.cpp")
if(GAME_TEST_SOURCES)
    add_executable(test_game ${GAME_TEST_SOURCES} ${NETWORK_PROTOCOL_SOURCES})
    target_link_libraries(test_game PRIVATE RType ECS raylib glfw dl)
    target_include_directories(test_game PRIVATE
        Games/RType
        Network/include
        ECS/include
        ${RTYPE_INCLUDE_DIRS}
    )
    add_test(NAME Game_Tests COMMAND test_game)
endif()

file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    file(GLOB_RECURSE NETWORK_IMPL_SOURCES "Network/*.cpp")
    list(FILTER NETWORK_IMPL_SOURCES EXCLUDE REGEX ".*/Network/test/.*")
    list(FILTER NETWORK_IMPL_SOURCES EXCLUDE REGEX ".*/Network/main.cpp")

    add_executable(test_network ${NETWORK_TEST_SOURCES} ${NETWORK_IMPL_SOURCES})
    target_include_directories(test_network PRIVATE Network Network/include)

    find_package(doctest CONFIG REQUIRED)
    target_link_libraries(test_network doctest::doctest)

    find_package(Threads REQUIRED)
    target_link_libraries(test_network Threads::Threads asio::asio)

    add_test(NAME Network_Tests COMMAND test_network)
endif()