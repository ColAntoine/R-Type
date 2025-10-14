# R-Type Game CMakeLists.txt
cmake_minimum_required(VERSION 3.21)

# Set the project name
set(RTYPE_PROJECT_NAME "RType")

# Collect all source files
file(GLOB_RECURSE RTYPE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Collect system source files separately
file(GLOB_RECURSE GAME_SYSTEM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Entity/Systems/*.cpp")

# Remove system sources from main library sources
list(REMOVE_ITEM RTYPE_SOURCES ${GAME_SYSTEM_SOURCES})

# Collect all header files  
file(GLOB_RECURSE RTYPE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Filter out test files if any
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER GAME_SYSTEM_SOURCES EXCLUDE REGEX ".*/test/.*")

# Create source groups for better IDE organization
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RTYPE_SOURCES} ${RTYPE_HEADERS})

# Define include directories for RType
set(RTYPE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/States
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Systems
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# Create RType library
add_library(${RTYPE_PROJECT_NAME} STATIC ${RTYPE_SOURCES})
set_target_properties(${RTYPE_PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)  # <--- add this

# Set include directories for the library
target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC
    ${RTYPE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/Network/include
)

# Link dependencies for RType
target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC
    raylib
    glfw
)

# Link with ECS library if available
if(TARGET ECS)
    target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC ECS)
    target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/ECS/include
    )
endif()

# Set C++ standard
target_compile_features(${RTYPE_PROJECT_NAME} PUBLIC cxx_std_20)

# Add compiler flags
target_compile_options(${RTYPE_PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# --- Determine shared library output directory depending on the platform ---
if (WIN32)
    # On Windows, DLLs are considered RUNTIME files
    set(GAME_SYSTEMS_BUILD_DIR "${CMAKE_BINARY_DIR}/bin/lib/systems")
else()
    # On Linux/macOS, shared libraries are considered LIBRARY files
    set(GAME_SYSTEMS_BUILD_DIR "${CMAKE_BINARY_DIR}/lib/systems")
endif()

# Automatically export all symbols on Windows (no need for __declspec(dllexport))
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# --- Build each game system as a shared library ---
foreach(SYSTEM_FILE ${GAME_SYSTEM_SOURCES})
    get_filename_component(SYSTEM_NAME ${SYSTEM_FILE} NAME_WE)
    set(GAME_SYSTEM_NAME "game_${SYSTEM_NAME}")

    add_library(${GAME_SYSTEM_NAME} SHARED ${SYSTEM_FILE})

    # Include directories for each system
    target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
        ${RTYPE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/Network/include
    )

    # Link dependencies
    target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC
        raylib
        glfw
        ${RTYPE_PROJECT_NAME}
    )

    # Link ECS if available
    if(TARGET ECS)
        target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC ECS)
        target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
            ${CMAKE_SOURCE_DIR}/ECS/include
        )
    endif()

    # Require C++20
    target_compile_features(${GAME_SYSTEM_NAME} PUBLIC cxx_std_20)

    # Set output directories for all platforms
    set_target_properties(${GAME_SYSTEM_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"   # .a or .lib import files
        LIBRARY_OUTPUT_DIRECTORY "${GAME_SYSTEMS_BUILD_DIR}"         # .so / .dylib
        RUNTIME_OUTPUT_DIRECTORY "${GAME_SYSTEMS_BUILD_DIR}"         # .dll (Windows)
    )

    message(STATUS "Building game system: ${GAME_SYSTEM_NAME} from ${SYSTEM_FILE}")
endforeach()

# --- Copy shared libraries to the runtime directory ---
add_custom_target(copy_game_systems ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${GAME_SYSTEMS_BUILD_DIR} ${CMAKE_BINARY_DIR}/bin/lib/systems
    DEPENDS ${GAME_SYSTEM_SOURCES}
    COMMENT "Copying game system libraries to binary directory"
)


# Create executable if this is built as standalone
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Find required dependencies when building standalone
    find_package(raylib CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    
    # Check if main.cpp exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
        add_executable(${RTYPE_PROJECT_NAME}_exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
        target_link_libraries(${RTYPE_PROJECT_NAME}_exe PRIVATE ${RTYPE_PROJECT_NAME})
        set_target_properties(${RTYPE_PROJECT_NAME}_exe PROPERTIES
            OUTPUT_NAME "r-type_client"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
    endif()
endif()

# Export the library for parent projects
set(RTYPE_LIBRARIES ${RTYPE_PROJECT_NAME} PARENT_SCOPE)
set(RTYPE_INCLUDE_DIRS ${RTYPE_INCLUDE_DIRS} PARENT_SCOPE)

# Optional: minimal configuration info (avoid printing long lists)
list(LENGTH RTYPE_SOURCES RTYPE_SOURCES_COUNT)
list(LENGTH GAME_SYSTEM_SOURCES GAME_SYSTEM_SOURCES_COUNT)
message(STATUS "RType Game Configuration: (sources=${RTYPE_SOURCES_COUNT}, systems=${GAME_SYSTEM_SOURCES_COUNT})")