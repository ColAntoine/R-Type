cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ECS_Library VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

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

# Add the main ECS shared library (components + core functionality)
add_library(ECS SHARED ${LIB_ECS_SOURCES})
set_target_properties(ECS PROPERTIES OUTPUT_NAME "ECS")

# Link dl library for dynamic loading
target_link_libraries(ECS dl)

# Ensure ECS target can include project Utils headers
target_include_directories(ECS PUBLIC ${CMAKE_SOURCE_DIR} ECS ECS/include)

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

# Create lib/systems directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems)

# Display build information
message(STATUS "Building ECS library and system plugins")
message(STATUS "  - libECS.so: Core ECS library with components, registry, and dynamic loader")
message(STATUS "  - lib/systems/libposition_system.so: Position update system")
message(STATUS "  - lib/systems/libcollision_system.so: Collision detection system")
message(STATUS "  - lib/systems/libsprite_system.so: Sprite rendering system")
message(STATUS "  - lib/systems/libanimation_system.so: Animation system")