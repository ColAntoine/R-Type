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
find_package(glfw3 QUIET)

if (NOT raylib_FOUND)
    message(WARNING "raylib not found. UI components will not be built.")
    set(BUILD_UI_COMPONENTS OFF)
else()
    message(STATUS "raylib found. UI components will be built.")
    set(BUILD_UI_COMPONENTS ON)
    
    if (NOT glfw3_FOUND)
        message(WARNING "glfw3 not found but required for raylib.")
    else()
        message(STATUS "glfw3 found.")
    endif()
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

# UI Component sources (to be included in main ECS library)
set(UI_COMPONENT_SOURCES
    src/UI/Components/Button.cpp
    src/UI/Components/InputField.cpp
    src/UI/Components/Panel.cpp
    src/UI/Components/Text.cpp
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
# Include UI components and system if raylib is available
if (BUILD_UI_COMPONENTS)
    add_library(ECS SHARED ${LIB_ECS_SOURCES} ${UI_COMPONENT_SOURCES} ${UI_SYSTEM_SOURCES})
    target_link_libraries(ECS dl raylib)
    
    # Link with glfw3 if available
    if(TARGET glfw)
        target_link_libraries(ECS glfw)
        message(STATUS "Linking ECS with glfw")
    endif()
    
    message(STATUS "Building ECS with UI components and UISystem")
else()
    add_library(ECS SHARED ${LIB_ECS_SOURCES})
    target_link_libraries(ECS dl)
    message(STATUS "Building ECS without UI components")
endif()

set_target_properties(ECS PROPERTIES OUTPUT_NAME "ECS")

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
    # UI components and UISystem are now part of the main ECS library
    # No need for separate libraries anymore
    
    # Create lib/ui directory for backward compatibility (if needed)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ui)

    message(STATUS "  - UI components and UISystem included in libECS.so")
endif()

# Create lib/systems directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems)

# Add UI test executable if raylib is available (commented out - test file is commented)
# if (BUILD_UI_COMPONENTS)
#     add_executable(ui_test test/ui_test.cpp)
#     target_link_libraries(ui_test ECS raylib)
#     message(STATUS "  - ui_test: UI system test executable")
# endif()

# Display build information
message(STATUS "Building ECS library and system plugins")
message(STATUS "  - libECS.so: Core ECS library with components, registry, and dynamic loader")
message(STATUS "  - lib/systems/libposition_system.so: Position update system")
message(STATUS "  - lib/systems/libcollision_system.so: Collision detection system")
message(STATUS "  - lib/systems/libsprite_system.so: Sprite rendering system")
message(STATUS "  - lib/systems/libanimation_system.so: Animation system")