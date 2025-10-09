cmake_minimum_required(VERSION 3.21)

# Project name and version
project(ecs VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(include)

# Source files for the ecs executable
set(ECS_CORE
    main.cpp
    src/registry.cpp
    src/systems.cpp
    src/dlloader.cpp
)

# Source files for the libcomponents.so shared library
set(LIB_COMPONENTS_SOURCES
    src/components/library.cpp
)

# Add the executable
add_executable(ecs ${ECS_CORE})

# Add the shared library
add_library(components SHARED ${LIB_COMPONENTS_SOURCES})
set_target_properties(components PROPERTIES OUTPUT_NAME "components")

# Set the runtime path for the executable to find the shared library
set_target_properties(ecs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ecs PROPERTIES INSTALL_RPATH "${CMAKE_BINARY_DIR}")

# Find and link Raylib
find_package(PkgConfig REQUIRED)

# Try to find raylib using pkg-config first
pkg_check_modules(RAYLIB raylib)

if(RAYLIB_FOUND)
    target_include_directories(ecs PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_libraries(ecs ${RAYLIB_LIBRARIES} dl)
    target_compile_options(ecs PRIVATE ${RAYLIB_CFLAGS_OTHER})
else()
    # Fallback: try to find raylib manually
    find_library(RAYLIB_LIBRARY
        NAMES raylib
        PATHS /usr/local/lib /usr/lib /usr/local/lib64 /usr/lib64
    )

    find_path(RAYLIB_INCLUDE_DIR
        NAMES raylib.h
        PATHS /usr/local/include /usr/include
    )

    if(RAYLIB_LIBRARY AND RAYLIB_INCLUDE_DIR)
        target_include_directories(ecs PRIVATE ${RAYLIB_INCLUDE_DIR})
        target_link_libraries(ecs ${RAYLIB_LIBRARY} dl)
    else()
        message(FATAL_ERROR "Could not find raylib library. Please install raylib development package.")
    endif()
endif()

# Link the shared library to the executable
add_dependencies(ecs components)