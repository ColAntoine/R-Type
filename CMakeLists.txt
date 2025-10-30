cmake_minimum_required(VERSION 3.21)

# ------------------------------------------------------------------------------
# Toolchain (auto-setup vcpkg if not provided)
# ------------------------------------------------------------------------------
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

# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------
project(R-Type)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pretty ASCII banner (pure cosmetic)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

# ------------------------------------------------------------------------------
# Dependencies (through vcpkg)
# ------------------------------------------------------------------------------
find_package(raylib CONFIG REQUIRED)   # raylib imported target (usually 'raylib')
find_package(asio   CONFIG REQUIRED)   # asio::asio
find_package(glfw3  CONFIG REQUIRED)   # glfw
# Threads may be needed by server/tests later
find_package(Threads REQUIRED)

# ------------------------------------------------------------------------------
# Global Windows compatibility layer (INTERFACE target)
# This ensures *all* targets that link to it get the same safe defines,
# preventing conflicts with raylib symbols (CloseWindow/ShowCursor/Rectangle)
# and winsock include order issues.
# ------------------------------------------------------------------------------
add_library(win_sanity INTERFACE)

if(WIN32)
    target_compile_definitions(win_sanity INTERFACE
        _WIN32_WINNT=0x0A00
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        NOGDI
        NOUSER
        # _WINSOCKAPI_  <-- REMOVE THIS LINE
    )
    if(MSVC)
        target_compile_options(win_sanity INTERFACE
            /Zc:__cplusplus
            /permissive-
            /U_WINSOCKAPI_       # <-- actively undef, just in case
        )
        # si tu as gardé la neutralisation de l’attribut:
        # target_compile_definitions(win_sanity INTERFACE __attribute_maybe_unused__=)
    endif()
endif()


# ------------------------------------------------------------------------------
# Subprojects
# ------------------------------------------------------------------------------
# ECS subproject (builds ECS shared lib and its plugins)
add_subdirectory(ECS)

# RType game module (defines RType lib/exes inside)
add_subdirectory(Games/RType)

# ------------------------------------------------------------------------------
# Copy runtime libs next to binaries (for Windows DLLs / plugins)
# ------------------------------------------------------------------------------
add_custom_target(copy_libs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/bin/lib
    COMMENT "Copying system libraries to binary directory"
)

# ------------------------------------------------------------------------------
# r-type_client (define here only if not already defined in Games/RType)
# ------------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/Games/RType/main_client.cpp" AND NOT TARGET r-type_client)
    add_executable(r-type_client "${CMAKE_SOURCE_DIR}/Games/RType/main_client.cpp")

    target_link_libraries(r-type_client PRIVATE
        RType
        ECS
        raylib
        glfw
        win_sanity   # <-- propagate safe Windows defines
    )

    if(UNIX)
        target_link_libraries(r-type_client PRIVATE dl)
    endif()

    target_include_directories(r-type_client PRIVATE
        Games/RType
        Games/RType/Core
        ECS/include
        ${RTYPE_INCLUDE_DIRS}
    )

    set_target_properties(r-type_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    add_dependencies(r-type_client copy_libs)
endif()

# ------------------------------------------------------------------------------
# r-type_server (define here only if not already defined in Games/RType)
# ------------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/Games/RType/main_server.cpp" AND NOT TARGET r-type_server)
    # Collect server sources (exclude any accidental main_*.cpp)
    file(GLOB_RECURSE SERVER_SOURCES
        "${CMAKE_SOURCE_DIR}/Games/RType/Core/Server/*.cpp"
    )
    list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*/main_.*\\.cpp")

    add_executable(r-type_server
        "${CMAKE_SOURCE_DIR}/Games/RType/main_server.cpp"
        ${SERVER_SOURCES}
    )

    target_include_directories(r-type_server PRIVATE
        Games/RType
        Games/RType/Core
        Games/RType/Core/Server
        ${CMAKE_SOURCE_DIR}
    )

    target_link_libraries(r-type_server PRIVATE
        Threads::Threads
        asio::asio
        win_sanity   # <-- propagate safe Windows defines
    )

    if(WIN32)
        target_link_libraries(r-type_server PRIVATE ws2_32 mswsock iphlpapi)
    endif()

    # Link game libs if present
    if(TARGET RType)
        target_link_libraries(r-type_server PRIVATE RType)
    endif()
    if(TARGET ECS)
        target_link_libraries(r-type_server PRIVATE ECS)
        target_include_directories(r-type_server PRIVATE ${CMAKE_SOURCE_DIR}/ECS/include)
    endif()

    set_target_properties(r-type_server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# ------------------------------------------------------------------------------
# Ensure ECS artefacts land under build/lib (so copy_libs can mirror them)
# Also propagate Windows defines to ECS as it exposes headers used by others.
# ------------------------------------------------------------------------------
if(TARGET ECS)
    set_target_properties(ECS PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
    target_link_libraries(ECS PRIVATE win_sanity)
endif()

# ------------------------------------------------------------------------------
# Tests (optional)
# ------------------------------------------------------------------------------
enable_testing()

# ECS tests (if any)
file(GLOB ECS_TEST_SOURCES "ECS/test/*.cpp")
if(ECS_TEST_SOURCES)
    add_executable(test_ecs ${ECS_TEST_SOURCES})
    target_include_directories(test_ecs PRIVATE
        ${CMAKE_SOURCE_DIR}/ECS/include
        ${CMAKE_SOURCE_DIR}/ECS
    )
    if(TARGET ECS)
        target_link_libraries(test_ecs PRIVATE ECS raylib win_sanity)
    else()
        target_link_libraries(test_ecs PRIVATE raylib win_sanity)
    endif()

    find_package(doctest CONFIG REQUIRED)
    target_link_libraries(test_ecs PRIVATE doctest::doctest)

    add_test(NAME ECS_Tests COMMAND test_ecs)
endif()

# (Deprecated) network tests
file(GLOB NETWORK_TEST_SOURCES "Network/test/*.cpp")
if(NETWORK_TEST_SOURCES)
    message(STATUS "Network tests found but Network folder is deprecated. Prefer Games/RType/Core/Server/test/")
    file(GLOB_RECURSE SERVER_IMPL_SOURCES "Games/RType/Core/Server/*.cpp")
    list(FILTER SERVER_IMPL_SOURCES EXCLUDE REGEX ".*/test/.*")
    list(FILTER SERVER_IMPL_SOURCES EXCLUDE REGEX ".*/main_.*\\.cpp")

    add_executable(test_network ${NETWORK_TEST_SOURCES} ${SERVER_IMPL_SOURCES})
    target_include_directories(test_network PRIVATE
        Games/RType/Core/Server
        Games/RType/Core
        Games/RType
        ${CMAKE_SOURCE_DIR}
    )

    if(TARGET ECS)
        target_include_directories(test_network PRIVATE ${CMAKE_SOURCE_DIR}/ECS/include)
        target_link_libraries(test_network PRIVATE ECS)
    endif()
    if(TARGET RType)
        target_link_libraries(test_network PRIVATE RType)
    endif()

    find_package(doctest CONFIG REQUIRED)
    target_link_libraries(test_network PRIVATE doctest::doctest)

    target_link_libraries(test_network PRIVATE
        Threads::Threads
        asio::asio
        win_sanity
    )
    if(WIN32)
        target_link_libraries(test_network PRIVATE ws2_32 mswsock iphlpapi)
    endif()

    add_test(NAME Network_Tests COMMAND test_network)
endif()
