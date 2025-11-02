cmake_minimum_required(VERSION 3.21)

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

        # Bootstrap vcpkg
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
    endif()

    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# ------------------------------------------------------------------
# Project
# ------------------------------------------------------------------
project(ECS_Library VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

include_directories(include ${CMAKE_SOURCE_DIR})

find_package(raylib QUIET)
find_package(glfw3 QUIET)

if(TARGET raylib)
    set(BUILD_UI_COMPONENTS ON)
    message(STATUS "raylib found. UI components will be built.")
    if(TARGET glfw OR TARGET glfw3::glfw)
        message(STATUS "glfw3 found.")
    else()
        message(WARNING "glfw3 not found but required for raylib UI parts.")
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

function(_prefix_lib_on_windows target)
    if(WIN32)
        set_target_properties(${target} PROPERTIES PREFIX "lib")
    endif()
endfunction()

# ------------------------------------------------------------------
# ECS shared library
# ------------------------------------------------------------------
if(BUILD_UI_COMPONENTS)
    add_library(ECS SHARED ${LIB_ECS_SOURCES} ${UI_COMPONENT_SOURCES})
    _prefix_lib_on_windows(ECS)

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
    _prefix_lib_on_windows(ECS)

    target_link_libraries(ECS PRIVATE
        $<$<NOT:$<PLATFORM_ID:Windows>>:dl>
    )
    message(STATUS "Building ECS without UI components")
endif()

target_include_directories(ECS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_compile_definitions(ECS PRIVATE
        _WIN32_WINNT=0x0A00
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        NOGDI
        _WINSOCKAPI_
    )
endif()

if(TARGET win_sanity)
    target_link_libraries(ECS PRIVATE win_sanity)
endif()

if(MSVC)
    set_target_properties(ECS PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set_target_properties(ECS PROPERTIES OUTPUT_NAME "ECS")

if(WIN32)
  set(_ECS_RUNTIME_NAME libECS.dll)
else()
  set(_ECS_RUNTIME_NAME libECS.so)
endif()

add_custom_command(TARGET ECS POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ECS> ${CMAKE_BINARY_DIR}/lib/${_ECS_RUNTIME_NAME}
  COMMENT "Copying $<TARGET_FILE_NAME:ECS> to ${CMAKE_BINARY_DIR}/lib/${_ECS_RUNTIME_NAME}"
)

# ------------------------------------------------------------------
# ECS system plugins (shared) if raylib available
# ------------------------------------------------------------------
if(BUILD_UI_COMPONENTS)
    function(_ecs_setup_plugin target)
        target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_link_libraries(${target} PRIVATE ECS raylib)

        _prefix_lib_on_windows(${target})

        if(TARGET win_sanity)
            target_link_libraries(${target} PRIVATE win_sanity)
        endif()

        foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
            set_target_properties(${target} PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/systems/${cfg}
                LIBRARY_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/systems/${cfg}
                RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib/systems/${cfg}
            )
        endforeach()

        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib/systems
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_BINARY_DIR}/lib/systems/$<TARGET_FILE_NAME:${target}>
            COMMENT "Copying $<TARGET_FILE_NAME:${target}> to ${CMAKE_BINARY_DIR}/lib/systems"
        )
    endfunction()

    add_library(position_system   SHARED ${POSITION_SYSTEM_SOURCES})
    set_target_properties(position_system PROPERTIES 
        OUTPUT_NAME "position_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    _ecs_setup_plugin(position_system)

    add_library(collision_system  SHARED ${COLLISION_SYSTEM_SOURCES})
    set_target_properties(collision_system PROPERTIES 
        OUTPUT_NAME "collision_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    _ecs_setup_plugin(collision_system)

    add_library(sprite_system     SHARED ${SPRITE_SYSTEM_SOURCES})
    set_target_properties(sprite_system PROPERTIES 
        OUTPUT_NAME "sprite_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    _ecs_setup_plugin(sprite_system)

    add_library(animation_system  SHARED ${ANIMATION_SYSTEM_SOURCES})
    set_target_properties(animation_system PROPERTIES 
        OUTPUT_NAME "animation_system"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
    _ecs_setup_plugin(animation_system)

    add_library(render_UISystem   SHARED ${UI_SYSTEM_SOURCES})
    set_target_properties(render_UISystem PROPERTIES 
        OUTPUT_NAME "render_UISystem"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
    )
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
# Tests with doctest
# ------------------------------------------------------------------
find_package(doctest CONFIG QUIET)

if(doctest_FOUND)
    message(STATUS "doctest found. Building unit tests.")
    enable_testing()
    
    # Test executable for AssetManager
    add_executable(asset_manager_test test/asset_manager_test.cpp)
    target_link_libraries(asset_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(asset_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME AssetManagerTest COMMAND asset_manager_test)
    
    # Test executable for RenderManager
    add_executable(render_manager_test test/render_manager_test.cpp)
    target_link_libraries(render_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(render_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME RenderManagerTest COMMAND render_manager_test)
    
    # Test executable for PhysicsManager
    add_executable(physics_manager_test test/physics_manager_test.cpp)
    target_link_libraries(physics_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(physics_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME PhysicsManagerTest COMMAND physics_manager_test)
    
    # Test executable for AudioManager
    add_executable(audio_manager_test test/audio_manager_test.cpp)
    target_link_libraries(audio_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(audio_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME AudioManagerTest COMMAND audio_manager_test)
    
    # Test executable for MessagingManager
    add_executable(messaging_manager_test test/messaging_manager_test.cpp)
    target_link_libraries(messaging_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(messaging_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME MessagingManagerTest COMMAND messaging_manager_test)
    
    # Test executable for ECS Core (Registry, Components, Systems, Zipper)
    add_executable(ecs_core_test test/ecs_core_test.cpp)
    target_link_libraries(ecs_core_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(ecs_core_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME ECSCoreTest COMMAND ecs_core_test)
    
    # Test executable for SpatialHash
    add_executable(spatial_hash_test test/spatial_hash_test.cpp)
    target_link_libraries(spatial_hash_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(spatial_hash_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME SpatialHashTest COMMAND spatial_hash_test)
    
    # Test executable for TextureManager
    add_executable(texture_manager_test test/texture_manager_test.cpp)
    target_link_libraries(texture_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(texture_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME TextureManagerTest COMMAND texture_manager_test)
    
    # Test executable for SoundManager
    add_executable(sound_manager_test test/sound_manager_test.cpp)
    target_link_libraries(sound_manager_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(sound_manager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME SoundManagerTest COMMAND sound_manager_test)
    
    # Test executable for EventBus
    add_executable(event_bus_test test/event_bus_test.cpp)
    target_link_libraries(event_bus_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(event_bus_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME EventBusTest COMMAND event_bus_test)
    
    # Test executable for Camera
    add_executable(camera_test test/camera_test.cpp)
    target_link_libraries(camera_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(camera_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME CameraTest COMMAND camera_test)
    
    # Test executable for SpriteBatch
    add_executable(sprite_batch_test test/sprite_batch_test.cpp)
    target_link_libraries(sprite_batch_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(sprite_batch_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME SpriteBatchTest COMMAND sprite_batch_test)
    
    # Test executable for UI Components
    add_executable(ui_components_test test/ui_components_test.cpp)
    target_link_libraries(ui_components_test PRIVATE ECS raylib doctest::doctest)
    target_include_directories(ui_components_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME UIComponentsTest COMMAND ui_components_test)
    
    message(STATUS "Unit tests configured:")
    message(STATUS "  - asset_manager_test")
    message(STATUS "  - render_manager_test")
    message(STATUS "  - physics_manager_test")
    message(STATUS "  - audio_manager_test")
    message(STATUS "  - messaging_manager_test")
    message(STATUS "  - ecs_core_test")
    message(STATUS "  - spatial_hash_test")
    message(STATUS "  - texture_manager_test")
    message(STATUS "  - sound_manager_test")
    message(STATUS "  - event_bus_test")
    message(STATUS "  - camera_test")
    message(STATUS "  - sprite_batch_test")
    message(STATUS "  - ui_components_test")
    message(STATUS "Run tests with: ctest --output-on-failure")
else()
    message(WARNING "doctest not found. Unit tests will NOT be built.")
    message(STATUS "To install doctest: vcpkg install doctest")
endif()

# ------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------
message(STATUS "Building ECS library and system plugins")
message(STATUS "  - ECS core library with components, registry, and dynamic loader")
message(STATUS "  - Systems will be built under lib/systems/<Config>/")
message(STATUS "  - UI components included if raylib was found")
