cmake_minimum_required(VERSION 3.21)

# =============================
#   VCPKG AUTOMATIC SETUP
# =============================
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

        message(STATUS "Installing vcpkg dependencies...")
        execute_process(
            COMMAND ${VCPKG_ROOT}/vcpkg install
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE install_result
        )
        if(NOT install_result EQUAL 0)
            message(FATAL_ERROR "Failed to install vcpkg dependencies")
        endif()
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# =============================
#   PROJECT SETUP
# =============================
project(RType)
set(CMAKE_CXX_STANDARD 17)

# Build ECS if not already built
if(NOT TARGET ECS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ECS ${CMAKE_BINARY_DIR}/ECS_build)
endif()

set(RTYPE_PROJECT_NAME "RType")

# =============================
#   SOURCE COLLECTION
# =============================
file(GLOB_RECURSE RTYPE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/external/.*")
list(APPEND RTYPE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Constants.cpp")

file(GLOB_RECURSE GAME_SYSTEM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Entity/Systems/*.cpp")
list(REMOVE_ITEM RTYPE_SOURCES ${GAME_SYSTEM_SOURCES})
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/main_client\\.cpp")
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/main_server\\.cpp")
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/Core/Server/.*")
list(APPEND RTYPE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Core/Server/Protocol/Protocol.cpp")

file(GLOB_RECURSE RTYPE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
list(FILTER RTYPE_HEADERS EXCLUDE REGEX ".*/external/.*")

list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER GAME_SYSTEM_SOURCES EXCLUDE REGEX ".*/test/.*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RTYPE_SOURCES} ${RTYPE_HEADERS})

# =============================
#   INCLUDE DIRECTORIES
# =============================
set(RTYPE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/States
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Systems
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Server
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Client
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# =============================
#   RType STATIC LIBRARY
# =============================
add_library(${RTYPE_PROJECT_NAME} STATIC ${RTYPE_SOURCES})
set_target_properties(${RTYPE_PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
add_definitions(-DREPO_ROOT="${REPO_ROOT}")
target_compile_definitions(${RTYPE_PROJECT_NAME} PRIVATE REPO_ROOT="${REPO_ROOT}")

target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC ${RTYPE_INCLUDE_DIRS})

target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC
    $<$<TARGET_EXISTS:raylib>:raylib>
    $<$<TARGET_EXISTS:glfw>:glfw>
    $<$<TARGET_EXISTS:win_sanity>:win_sanity>
    ECS
)
target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/include
)

target_compile_features(${RTYPE_PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${RTYPE_PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# =============================
#   BUILD GAME SYSTEMS (PLUGINS)
# =============================
foreach(SYSTEM_FILE ${GAME_SYSTEM_SOURCES})
    get_filename_component(SYSTEM_NAME ${SYSTEM_FILE} NAME_WE)
    set(GAME_SYSTEM_NAME "game_${SYSTEM_NAME}")

    add_library(${GAME_SYSTEM_NAME} SHARED ${SYSTEM_FILE})

    target_include_directories(${GAME_SYSTEM_NAME} PUBLIC ${RTYPE_INCLUDE_DIRS})
    target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC
        $<$<TARGET_EXISTS:raylib>:raylib>
        $<$<TARGET_EXISTS:glfw>:glfw>
        $<$<TARGET_EXISTS:win_sanity>:win_sanity>
        ${RTYPE_PROJECT_NAME}
        ECS
    )
    target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/include
    )
    target_compile_features(${GAME_SYSTEM_NAME} PUBLIC cxx_std_20)

    set(SYSTEM_OUTPUT_DIR ${CMAKE_BINARY_DIR}/lib/systems)
    set_target_properties(${GAME_SYSTEM_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${SYSTEM_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${SYSTEM_OUTPUT_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${SYSTEM_OUTPUT_DIR}
    )

    if(WIN32)
        set_target_properties(${GAME_SYSTEM_NAME} PROPERTIES PREFIX "lib")
        add_custom_command(TARGET ${GAME_SYSTEM_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${GAME_SYSTEM_NAME}>"
                "${SYSTEM_OUTPUT_DIR}/"
        )
    endif()

    message(STATUS "Building game system: ${GAME_SYSTEM_NAME} from ${SYSTEM_FILE}")
endforeach()

# =============================
#   DEPENDENCIES
# =============================
find_package(raylib QUIET)
find_package(glfw3 QUIET)
find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

# =============================
#   CLIENT EXECUTABLE
# =============================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main_client.cpp")
    add_executable(r-type_client main_client.cpp)
    target_link_libraries(r-type_client PRIVATE
        ${RTYPE_PROJECT_NAME}
        ECS
        $<$<TARGET_EXISTS:raylib>:raylib>
        $<$<TARGET_EXISTS:glfw>:glfw>
        $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:dl>
    )
    target_include_directories(r-type_client PRIVATE
        ${RTYPE_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/include
    )
    set_target_properties(r-type_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../
    )
endif()

# =============================
#   SERVER EXECUTABLE
# =============================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main_server.cpp")
    file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Core/Server/*.cpp")
    list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*/main_.*\\.cpp")

    add_executable(r-type_server main_server.cpp ${SERVER_SOURCES})
    target_link_libraries(r-type_server PRIVATE
        ${RTYPE_PROJECT_NAME}
        ECS
        Threads::Threads
        asio::asio
    )
    target_include_directories(r-type_server PRIVATE
        ${RTYPE_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/include
    )
    set_target_properties(r-type_server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../
    )
endif()

# =============================
#   SUMMARY MESSAGE
# =============================
list(LENGTH RTYPE_SOURCES RTYPE_SOURCES_COUNT)
list(LENGTH GAME_SYSTEM_SOURCES GAME_SYSTEM_SOURCES_COUNT)
message(STATUS "RType Game Configuration: (sources=${RTYPE_SOURCES_COUNT}, systems=${GAME_SYSTEM_SOURCES_COUNT})")
