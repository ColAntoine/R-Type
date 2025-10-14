cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ecs VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows: automatically export all symbols for DLLs when no __declspec(dllexport) is used
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Include directories
include_directories(include)

# Source files for the ecs executable
set(ECS_CORE
    main.cpp
    src/registry.cpp
    src/systems.cpp
    src/dlloader.cpp
)

# Source files for the libcomponents shared library
set(LIB_COMPONENTS_SOURCES
    src/components/library.cpp
)

# Add the shared library
add_library(components SHARED ${LIB_COMPONENTS_SOURCES})
set_target_properties(components PROPERTIES OUTPUT_NAME "components")

# Consistent output directories across platforms
# - ARCHIVE: .a (Linux/macOS) or import .lib (Windows)
# - LIBRARY: .so (Linux) / .dylib (macOS)
# - RUNTIME: .dll (Windows)
set_target_properties(components PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add the executable
add_executable(ecs ${ECS_CORE})

# Place the executable in bin/
set_target_properties(ecs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Portable rpath so the exe in bin/ can find libs in ../lib at build/run time
if (APPLE)
    # @loader_path points to the directory of the running binary
    set_target_properties(ecs PROPERTIES
        BUILD_RPATH "@loader_path/../lib"
        INSTALL_RPATH "@loader_path/../lib"
    )
elseif (UNIX)
    # $ORIGIN points to the directory of the running binary
    set_target_properties(ecs PROPERTIES
        BUILD_RPATH "\$ORIGIN/../lib"
        INSTALL_RPATH "\$ORIGIN/../lib"
    )
endif()

# Find and link Raylib
find_package(PkgConfig REQUIRED)

# Try to find raylib using pkg-config first
pkg_check_modules(RAYLIB raylib)

if(RAYLIB_FOUND)
    target_include_directories(ecs PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_libraries(ecs ${RAYLIB_LIBRARIES})
    target_compile_options(ecs PRIVATE ${RAYLIB_CFLAGS_OTHER})
    # Link libdl only on Linux (not needed on macOS/Windows)
    if (UNIX AND NOT APPLE)
        target_link_libraries(ecs dl)
    endif()
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
        target_link_libraries(ecs ${RAYLIB_LIBRARY})
        # Link libdl only on Linux (not needed on macOS/Windows)
        if (UNIX AND NOT APPLE)
            target_link_libraries(ecs dl)
        endif()
    else()
        message(FATAL_ERROR "Could not find raylib library. Please install raylib development package.")
    endif()
endif()

# Ensure the executable is built after the shared library
add_dependencies(ecs components)
