cmake_minimum_required(VERSION 3.21)

# Project name and version
project(ECS VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Find Dependencies
# ============================================================================
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# ============================================================================
# Build ECS Core Library (libECS.so)
# ============================================================================

# Collect ECS core source files (exclude systems and tests)
file(GLOB_RECURSE ECS_CORE_SOURCES
    "src/registry.cpp"
    "src/dlloader.cpp"
    "src/library.cpp"
    "src/components/*.cpp"
)

# Filter out test files
list(FILTER ECS_CORE_SOURCES EXCLUDE REGEX ".*/test/.*")

# Create ECS core shared library
add_library(ECS SHARED ${ECS_CORE_SOURCES})

target_include_directories(ECS PUBLIC
    include
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ECS PUBLIC
    raylib
    glfw
    dl
)

# Set library properties
set_target_properties(ECS PROPERTIES
    OUTPUT_NAME "ECS"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    POSITION_INDEPENDENT_CODE ON
)

foreach(SYSTEM ${SYSTEMS})
    if(TARGET ${SYSTEM})
        set_target_properties(${SYSTEM} PROPERTIES
            POSITION_INDEPENDENT_CODE ON
        )
    endif()
endforeach()

# ============================================================================
# Build Individual System Libraries
# ============================================================================

# Find all system source files
file(GLOB SYSTEM_FILES "src/systems/*.cpp")

foreach(SYSTEM_FILE ${SYSTEM_FILES})
    # Get system name from filename
    get_filename_component(SYSTEM_NAME ${SYSTEM_FILE} NAME_WE)
    
    # Create system library
    add_library(${SYSTEM_NAME} SHARED ${SYSTEM_FILE})
    
    # Set include directories
    target_include_directories(${SYSTEM_NAME} PRIVATE
        include
        src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    
    # Link with ECS core and dependencies
    target_link_libraries(${SYSTEM_NAME} PUBLIC
        ECS
        raylib
        glfw
    )
    
    # Set system library properties
    set_target_properties(${SYSTEM_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Add to parent's copy_libs target if it exists
    if(TARGET copy_libs)
        add_custom_command(TARGET copy_libs POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                $<TARGET_FILE:${SYSTEM_NAME}> 
                ${CMAKE_BINARY_DIR}/bin/systems/
        )
    endif()
endforeach()