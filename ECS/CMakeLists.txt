cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ECS_Library VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Find raylib (required for UI components)
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    message(WARNING "raylib not found. UI components will not be built.")
    set(BUILD_UI_COMPONENTS OFF)
else()
    message(STATUS "raylib found. UI components will be built.")
    set(BUILD_UI_COMPONENTS ON)
endif()

# Source files for the libECS.so shared library (components + registry + dlloader)
set(LIB_ECS_SOURCES
    src/library.cpp
    src/registry.cpp
    src/dlloader.cpp
    src/components/position.cpp
    src/components/velocity.cpp
    src/components/sprite.cpp
    src/components/collider.cpp
)

# UI Component sources
set(UI_COMPONENT_SOURCES
    src/UI/Components/Button.cpp
    src/UI/Components/InputField.cpp
)

# Source files for system libraries
set(POSITION_SYSTEM_SOURCES
    src/systems/position_system.cpp
)

set(COLLISION_SYSTEM_SOURCES
    src/systems/collision_system.cpp
)

set(SPRITE_SYSTEM_SOURCES
    src/systems/sprite_system.cpp
)

set(ANIMATION_SYSTEM_SOURCES
    src/systems/animation_system.cpp
)

set(UI_SYSTEM_SOURCES
    src/systems/UISystem.cpp
)

# Add the main ECS shared library (components + core functionality)
add_library(ECS SHARED ${LIB_ECS_SOURCES})
set_target_properties(ECS PROPERTIES OUTPUT_NAME "ECS")

# Link dl library for dynamic loading
target_link_libraries(ECS dl)

# Add system libraries
add_library(position_system SHARED ${POSITION_SYSTEM_SOURCES})
set_target_properties(position_system PROPERTIES
    OUTPUT_NAME "position_system"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
)

add_library(collision_system SHARED ${COLLISION_SYSTEM_SOURCES})
set_target_properties(collision_system PROPERTIES
    OUTPUT_NAME "collision_system"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
)

add_library(sprite_system SHARED ${SPRITE_SYSTEM_SOURCES})
set_target_properties(sprite_system PROPERTIES
    OUTPUT_NAME "sprite_system"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
)

add_library(animation_system SHARED ${ANIMATION_SYSTEM_SOURCES})
set_target_properties(animation_system PROPERTIES
    OUTPUT_NAME "animation_system"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
)

# Build UI components and system only if raylib is found
if (BUILD_UI_COMPONENTS)
    # Add UI components library
    add_library(ui_components SHARED ${UI_COMPONENT_SOURCES})
    set_target_properties(ui_components PROPERTIES
        OUTPUT_NAME "ui_components"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/ui"
    )
    target_link_libraries(ui_components raylib)

    # Add UI system
    add_library(ui_system SHARED ${UI_SYSTEM_SOURCES})
    set_target_properties(ui_system PROPERTIES
        OUTPUT_NAME "ui_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    target_link_libraries(ui_system raylib)

    # Create lib/ui directory
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ui)

    message(STATUS "  - lib/ui/libui_components.so: UI components library")
    message(STATUS "  - lib/systems/libui_system.so: UI management system")
endif()

# Create lib/systems directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems)

# Add UI test executable if raylib is available
if (BUILD_UI_COMPONENTS)
    add_executable(ui_test test/ui_test.cpp)
    target_link_libraries(ui_test ECS ui_components ui_system raylib)
    message(STATUS "  - ui_test: UI system test executable")
endif()

# Display build information
message(STATUS "Building ECS library and system plugins")
message(STATUS "  - libECS.so: Core ECS library with components, registry, and dynamic loader")
message(STATUS "  - lib/systems/libposition_system.so: Position update system")
message(STATUS "  - lib/systems/libcollision_system.so: Collision detection system")
message(STATUS "  - lib/systems/libsprite_system.so: Sprite rendering system")
message(STATUS "  - lib/systems/libanimation_system.so: Animation system")