cmake_minimum_required(VERSION 3.21)

# =============================
#   VCPKG AUTOMATIC SETUP
# =============================
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

        message(STATUS "Installing vcpkg dependencies...")
        execute_process(
            COMMAND ${VCPKG_ROOT}/vcpkg install
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE install_result
        )
        if(NOT install_result EQUAL 0)
            message(FATAL_ERROR "Failed to install vcpkg dependencies")
        endif()
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Project name
project(Pang_Game VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Print Pang game banner
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "========================================")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "     PANG GAME - ECS Implementation")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "========================================")

# Find required packages (only if not already found)
if(NOT TARGET raylib)
    find_package(raylib CONFIG REQUIRED)
endif()

if(NOT TARGET glfw)
    find_package(glfw3 CONFIG REQUIRED)
endif()

# Find or add ECS library
if(NOT TARGET ECS)
    # If ECS is not already a target, add it as a subdirectory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/CMakeLists.txt")
        # Add the Utils include directory to the include path
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ECS ${CMAKE_CURRENT_BINARY_DIR}/ECS)
    else()
        message(FATAL_ERROR "ECS library not found. Make sure ../../ECS exists.")
    endif()
endif()

# Collect all game source files (excluding systems)
file(GLOB_RECURSE PANG_CORE_SOURCES 
    "Core/*.cpp"
)

file(GLOB_RECURSE PANG_COMPONENT_SOURCES 
    "Entity/Components/*.cpp"
)

# Combine all sources (excluding main.cpp and systems)
set(PANG_SOURCES
    ${PANG_CORE_SOURCES}
    ${PANG_COMPONENT_SOURCES}
)

# Remove main.cpp from the library sources if it exists
list(FILTER PANG_SOURCES EXCLUDE REGEX "main\\.cpp$")

# Collect all system files for dynamic libraries
file(GLOB PANG_SYSTEM_FILES "Entity/Systems/*/*.cpp")

# Include directories for Pang game
set(PANG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Create Pang library (static) only if there are sources
if(PANG_SOURCES)
    add_library(Pang STATIC ${PANG_SOURCES})
    
    # Enable position-independent code for static library (needed for shared library linking)
    set_target_properties(Pang PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    
    target_include_directories(Pang PUBLIC
        ${PANG_INCLUDE_DIRS}
    )
    
    # Link with ECS and raylib
    target_link_libraries(Pang PUBLIC
        ECS
        raylib
        glfw
        dl
    )
    
    message(STATUS "Pang game library created with ${PANG_SOURCES}")
else()
    message(STATUS "No source files found for Pang game library - will build executable without library")
endif()

# Build each system as a shared library
set(PANG_SYSTEM_COUNT 0)
foreach(SYSTEM_FILE ${PANG_SYSTEM_FILES})
    get_filename_component(SYSTEM_DIR ${SYSTEM_FILE} DIRECTORY)
    get_filename_component(SYSTEM_NAME ${SYSTEM_DIR} NAME)
    
    # Create library name: pang_SystemName
    set(LIB_NAME "pang_${SYSTEM_NAME}")
    
    add_library(${LIB_NAME} SHARED ${SYSTEM_FILE})
    
    target_include_directories(${LIB_NAME} PRIVATE
        ${PANG_INCLUDE_DIRS}
    )
    
    # Link with Pang library and ECS
    target_link_libraries(${LIB_NAME} PRIVATE
        Pang
        ECS
        raylib
    )
    
    # Set output directory for system libraries
    set_target_properties(${LIB_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
        PREFIX "lib"
    )
    
    math(EXPR PANG_SYSTEM_COUNT "${PANG_SYSTEM_COUNT}+1")
    message(STATUS "-- Building game system: ${LIB_NAME} from ${SYSTEM_FILE}")
endforeach()

# Custom target to copy Pang system libraries (only if systems exist)
if(PANG_SYSTEM_COUNT GREATER 0)
    add_custom_target(copy_pang_systems ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib/systems ${CMAKE_BINARY_DIR}/bin/lib/systems
        COMMENT "Copying Pang system libraries to binary directory"
    )
    
    # Make sure copy happens after all systems are built
    foreach(SYSTEM_FILE ${PANG_SYSTEM_FILES})
        get_filename_component(SYSTEM_DIR ${SYSTEM_FILE} DIRECTORY)
        get_filename_component(SYSTEM_NAME ${SYSTEM_DIR} NAME)
        set(LIB_NAME "pang_${SYSTEM_NAME}")
        add_dependencies(copy_pang_systems ${LIB_NAME})
    endforeach()
endif()

# Build Pang executable
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    add_executable(pang main.cpp)
    
    # Link with Pang library if it exists, otherwise just ECS
    if(TARGET Pang)
        target_link_libraries(pang PRIVATE
            Pang
            ECS
            raylib
            glfw
            dl
        )
    else()
        target_link_libraries(pang PRIVATE
            ECS
            raylib
            glfw
            dl
        )
    endif()
    
    target_include_directories(pang PRIVATE
        ${PANG_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../ECS/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
    )
    
    set_target_properties(pang PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    
    message(STATUS "Pang executable configured")
endif()

# Print configuration summary
message(STATUS "Pang Game Configuration: (sources=${PANG_SOURCES}, systems=${PANG_SYSTEM_COUNT})")
