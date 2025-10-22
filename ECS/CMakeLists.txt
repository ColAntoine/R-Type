cmake_minimum_required(VERSION 3.21)

# Project name and version
project(ECS_Library VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include ${CMAKE_SOURCE_DIR})

# Only find packages if not already found by parent CMakeLists
if(NOT TARGET raylib)
    find_package(raylib QUIET)
endif()

if(NOT TARGET glfw)
    find_package(glfw3 QUIET)
endif()

# Check if raylib is available (either found here or in parent)
if(TARGET raylib)
    set(BUILD_UI_COMPONENTS ON)
    message(STATUS "raylib found. UI components will be built.")
    
    if(TARGET glfw)
        message(STATUS "glfw3 found.")
    else()
        message(WARNING "glfw3 not found but required for raylib.")
    endif()
else()
    set(BUILD_UI_COMPONENTS OFF)
    message(WARNING "raylib not found. UI components will not be built.")
endif()

# Only find packages if not already found by parent CMakeLists
if(NOT TARGET raylib)
    find_package(raylib QUIET)
endif()

if(NOT TARGET glfw)
    find_package(glfw3 QUIET)
endif()

# Check if raylib is available (either found here or in parent)
if(TARGET raylib)
    set(BUILD_UI_COMPONENTS ON)
    message(STATUS "raylib found. UI components will be built.")
    
    if(TARGET glfw)
        message(STATUS "glfw3 found.")
    else()
        message(WARNING "glfw3 not found but required for raylib.")
    endif()
else()
    set(BUILD_UI_COMPONENTS OFF)
    message(WARNING "raylib not found. UI components will not be built.")
endif()


# Source files for the libECS.so shared library (components + registry + dlloader + assetmanager + renderer + physics + audio + messaging + plugin)
set(LIB_ECS_SOURCES
    src/library.cpp
    src/registry.cpp
    src/dlloader.cpp
    src/components/position.cpp
    src/components/velocity.cpp
    src/components/sprite.cpp
    src/components/collider.cpp
    src/AssetManager/AssetManager.cpp
    src/AssetManager/Texture/TextureManager.cpp
    src/AssetManager/Sound/SoundManager.cpp
    src/Renderer/RenderManager.cpp
    src/Renderer/Camera/Camera.cpp
    src/Renderer/Batch/SpriteBatch.cpp
    src/Physics/PhysicsManager.cpp
    src/Physics/Collision/CollisionDetector.cpp
    src/Physics/SpatialHash/SpatialHash.cpp
    src/Audio/AudioManager.cpp
    src/Audio/Music/MusicPlayer.cpp
    src/Audio/SFX/SFXPlayer.cpp
    src/Messaging/MessagingManager.cpp
    src/Messaging/EventBus.cpp
    src/Messaging/CommandDispatcher.cpp
    src/Messaging/MessageQueue.cpp
    src/Plugin/PluginManager.cpp
    src/Plugin/Plugin.cpp
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
# Include UI components but NOT UISystem
if (BUILD_UI_COMPONENTS)
    add_library(ECS SHARED ${LIB_ECS_SOURCES} ${UI_COMPONENT_SOURCES})
    target_link_libraries(ECS dl raylib)
    
    # Link with glfw3 if available
    if(TARGET glfw)
        target_link_libraries(ECS glfw)
        message(STATUS "Linking ECS with glfw")
    endif()
    
    message(STATUS "Building ECS with UI components")
else()
    add_library(ECS SHARED ${LIB_ECS_SOURCES})
    target_link_libraries(ECS dl)
    message(STATUS "Building ECS without UI components")
endif()

set_target_properties(ECS PROPERTIES OUTPUT_NAME "ECS")

# Add system libraries (only if raylib is available since they depend on it)
if (BUILD_UI_COMPONENTS)
    add_library(position_system SHARED ${POSITION_SYSTEM_SOURCES})
    set_target_properties(position_system PROPERTIES
        OUTPUT_NAME "position_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    target_include_directories(position_system PRIVATE include)
    target_link_libraries(position_system PRIVATE ECS raylib)

    add_library(collision_system SHARED ${COLLISION_SYSTEM_SOURCES})
    set_target_properties(collision_system PROPERTIES
        OUTPUT_NAME "collision_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    target_include_directories(collision_system PRIVATE include)
    target_link_libraries(collision_system PRIVATE ECS raylib)

    add_library(sprite_system SHARED ${SPRITE_SYSTEM_SOURCES})
    set_target_properties(sprite_system PROPERTIES
        OUTPUT_NAME "sprite_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    target_include_directories(sprite_system PRIVATE include)
    target_link_libraries(sprite_system PRIVATE ECS raylib)

    add_library(animation_system SHARED ${ANIMATION_SYSTEM_SOURCES})
    set_target_properties(animation_system PROPERTIES
        OUTPUT_NAME "animation_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    target_include_directories(animation_system PRIVATE include)
    target_link_libraries(animation_system PRIVATE ECS raylib)

    # ✅ UISystem as plugin
    add_library(render_UISystem SHARED ${UI_SYSTEM_SOURCES})
    set_target_properties(render_UISystem PROPERTIES
        OUTPUT_NAME "render_UISystem"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    target_include_directories(render_UISystem PRIVATE include)
    target_link_libraries(render_UISystem PRIVATE ECS raylib)
    
    message(STATUS "Building ECS system plugins with raylib support")
else()
    message(STATUS "Skipping ECS system plugins (raylib not available)")
endif()

# Build UI components and system only if raylib is found
if (BUILD_UI_COMPONENTS)
    # UI components are part of the main ECS library
    # UISystem is now a separate plugin
    
    # Create lib/ui directory for backward compatibility (if needed)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ui)

    message(STATUS "  - UI components included in libECS.so")
    message(STATUS "  - UISystem built as lib/systems/librender_UISystem.so")
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
message(STATUS "  - lib/systems/librender_UISystem.so: UI system plugin")