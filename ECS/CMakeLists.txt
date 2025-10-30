cmake_minimum_required(VERSION 3.21)

# ------------------------------------------------------------------
# Project
# ------------------------------------------------------------------
project(ECS_Library VERSION 1.0 LANGUAGES CXX)

# C++ standard (laisse 17 si ton code est écrit pour C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Public include dir (headers ECS/) + accès au ${CMAKE_SOURCE_DIR} si utile
# Remplacé par target_include_directories plus bas (meilleure pratique)
include_directories(include ${CMAKE_SOURCE_DIR})

# ------------------------------------------------------------------
# Dependencies (raylib + glfw3 via vcpkg)
# ------------------------------------------------------------------
if(NOT TARGET raylib)
    find_package(raylib QUIET)
endif()

if(NOT TARGET glfw AND NOT TARGET glfw3::glfw)
    find_package(glfw3 QUIET)
endif()

# Build UI parts only if raylib is present
if(TARGET raylib)
    set(BUILD_UI_COMPONENTS ON)
    message(STATUS "raylib found. UI components will be built.")
    if(TARGET glfw OR TARGET glfw3::glfw)
        message(STATUS "glfw3 found.")
    else()
        message(WARNING "glfw3 not found but required for raylib.")
    endif()
else()
    set(BUILD_UI_COMPONENTS OFF)
    message(WARNING "raylib not found. UI components will NOT be built.")
endif()

# ------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------
set(LIB_ECS_SOURCES
    src/library.cpp
    src/registry.cpp
    src/ALoader.cpp
    src/WinLoader.cpp
    src/LinuxLoader.cpp
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
)

set(UI_COMPONENT_SOURCES
    src/UI/Components/Button.cpp
    src/UI/Components/InputField.cpp
    src/UI/Components/Panel.cpp
    src/UI/Components/Text.cpp
)

set(POSITION_SYSTEM_SOURCES   src/systems/position_system.cpp)
set(COLLISION_SYSTEM_SOURCES  src/systems/collision_system.cpp)
set(SPRITE_SYSTEM_SOURCES     src/systems/sprite_system.cpp)
set(ANIMATION_SYSTEM_SOURCES  src/systems/animation_system.cpp)
set(UI_SYSTEM_SOURCES         src/systems/UISystem.cpp)

# ------------------------------------------------------------------
# ECS shared library
# ------------------------------------------------------------------
if(BUILD_UI_COMPONENTS)
    add_library(ECS SHARED ${LIB_ECS_SOURCES} ${UI_COMPONENT_SOURCES})
    target_link_libraries(ECS PRIVATE
        raylib
        $<$<NOT:$<PLATFORM_ID:Windows>>:dl>
    )
    if(TARGET glfw)
        target_link_libraries(ECS PRIVATE glfw)
        message(STATUS "Linking ECS with glfw")
    elseif(TARGET glfw3::glfw)
        target_link_libraries(ECS PRIVATE glfw3::glfw)
        message(STATUS "Linking ECS with glfw3::glfw")
    else()
        message(WARNING "glfw target not found (but raylib was found).")
    endif()
    message(STATUS "Building ECS with UI components")
else()
    add_library(ECS SHARED ${LIB_ECS_SOURCES})
    target_link_libraries(ECS PRIVATE
        $<$<NOT:$<PLATFORM_ID:Windows>>:dl>
    )
    message(STATUS "Building ECS without UI components")
endif()

# Public include pour les consommateurs de ECS
target_include_directories(ECS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows-safe defines (évite conflits Rectangle/CloseWindow/ShowCursor + winsock)
if(WIN32)
    target_compile_definitions(ECS PRIVATE
        _WIN32_WINNT=0x0A00
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        NOGDI
        _WINSOCKAPI_
    )
endif()

# Si l’interface "win_sanity" (définie au root) existe, on en hérite
if(TARGET win_sanity)
    target_link_libraries(ECS PRIVATE win_sanity)
endif()

# MSVC: génère l'import lib même s'il n'y a pas d'__declspec(dllexport)
if(MSVC)
    set_target_properties(ECS PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Nom de sortie de la DLL
set_target_properties(ECS PROPERTIES OUTPUT_NAME "ECS")

# Harmoniser les répertoires de sortie (multi-config: Debug/Release/…)
# ARCHIVE = .lib (import lib), LIBRARY/RUNTIME = .dll
foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    set_target_properties(ECS PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/${cfg}
        LIBRARY_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/${cfg}
        RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/${cfg}
    )
endforeach()

# ------------------------------------------------------------------
# ECS system plugins (shared) si raylib est dispo
# ------------------------------------------------------------------
if(BUILD_UI_COMPONENTS)
    # Helper pour appliquer les mêmes propriétés/links à chaque plugin
    function(_ecs_setup_plugin target)
        target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_link_libraries(${target} PRIVATE ECS raylib)

        # Hérite des defines Windows safe si dispo
        if(TARGET win_sanity)
            target_link_libraries(${target} PRIVATE win_sanity)
        endif()

        # Répertoires de sortie (multi-config)
        foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
            set_target_properties(${target} PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/systems/${cfg}
                LIBRARY_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/systems/${cfg}
                RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/systems/${cfg}
            )
        endforeach()
    endfunction()

    add_library(position_system   SHARED ${POSITION_SYSTEM_SOURCES})
    set_target_properties(position_system PROPERTIES OUTPUT_NAME "position_system")
    _ecs_setup_plugin(position_system)

    add_library(collision_system  SHARED ${COLLISION_SYSTEM_SOURCES})
    set_target_properties(collision_system PROPERTIES OUTPUT_NAME "collision_system")
    _ecs_setup_plugin(collision_system)

    add_library(sprite_system     SHARED ${SPRITE_SYSTEM_SOURCES})
    set_target_properties(sprite_system PROPERTIES OUTPUT_NAME "sprite_system")
    _ecs_setup_plugin(sprite_system)

    add_library(animation_system  SHARED ${ANIMATION_SYSTEM_SOURCES})
    set_target_properties(animation_system PROPERTIES OUTPUT_NAME "animation_system")
    _ecs_setup_plugin(animation_system)

    add_library(render_UISystem   SHARED ${UI_SYSTEM_SOURCES})
    set_target_properties(render_UISystem PROPERTIES OUTPUT_NAME "render_UISystem")
    _ecs_setup_plugin(render_UISystem)

    message(STATUS "Building ECS system plugins with raylib support")
else()
    message(STATUS "Skipping ECS system plugins (raylib not available)")
endif()

# ------------------------------------------------------------------
# Ensure dirs exist for single-config generators
# ------------------------------------------------------------------
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/ui)

# ------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------
message(STATUS "Building ECS library and system plugins")
message(STATUS "  - ECS core library with components, registry, and dynamic loader")
message(STATUS "  - Systems will be built under lib/systems/<Config>/")
message(STATUS "  - UI components included if raylib was found")
