# R-Type Game CMakeLists.txt
cmake_minimum_required(VERSION 3.21)

# Set the project name
set(RTYPE_PROJECT_NAME "RType")

# Collect all source files
file(GLOB_RECURSE RTYPE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Collect system source files separately
file(GLOB_RECURSE GAME_SYSTEM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Entity/Systems/*.cpp")

# Remove system sources from main library sources
list(REMOVE_ITEM RTYPE_SOURCES ${GAME_SYSTEM_SOURCES})

# Collect all header files  
file(GLOB_RECURSE RTYPE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Filter out test files if any
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER GAME_SYSTEM_SOURCES EXCLUDE REGEX ".*/test/.*")

# Create source groups for better IDE organization
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RTYPE_SOURCES} ${RTYPE_HEADERS})

# Define include directories for RType
set(RTYPE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/States
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Systems
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# Create RType library
add_library(${RTYPE_PROJECT_NAME} STATIC ${RTYPE_SOURCES})
set_target_properties(${RTYPE_PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)  # <--- add this

# Set include directories for the library
target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC
    ${RTYPE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/Network/include
)

# Link dependencies for RType
target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC
    raylib
    glfw
)

# Link with ECS library if available
if(TARGET ECS)
    target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC ECS)
    target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/ECS/include
    )
endif()

# Set C++ standard
target_compile_features(${RTYPE_PROJECT_NAME} PUBLIC cxx_std_20)

# Add compiler flags
target_compile_options(${RTYPE_PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Build individual game system libraries as .so files
foreach(SYSTEM_FILE ${GAME_SYSTEM_SOURCES})
    get_filename_component(SYSTEM_NAME ${SYSTEM_FILE} NAME_WE)
    
    # Create a unique name for game systems to avoid conflicts with ECS systems
    set(GAME_SYSTEM_NAME "game_${SYSTEM_NAME}")
    
    add_library(${GAME_SYSTEM_NAME} SHARED ${SYSTEM_FILE})
    
    # Set include directories for the system
    target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
        ${RTYPE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/Network/include
    )
    
    # Link with dependencies
    target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC
        raylib
        glfw
        ${RTYPE_PROJECT_NAME}          # <--- add this
    )
    
    # Link with ECS library if available
    if(TARGET ECS)
        target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC ECS)
        target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
            ${CMAKE_SOURCE_DIR}/ECS/include
        )
    endif()
    
    # Set C++ standard for systems
    target_compile_features(${GAME_SYSTEM_NAME} PUBLIC cxx_std_20)
    
    # Set output directory for game system libraries
    set_target_properties(${GAME_SYSTEM_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems
    )
    
    message(STATUS "Building game system: ${GAME_SYSTEM_NAME} from ${SYSTEM_FILE}")
endforeach()

# Add custom target to copy game system libraries to binary directory
add_custom_target(copy_game_systems ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib/systems ${CMAKE_BINARY_DIR}/bin/lib/systems
    DEPENDS ${GAME_SYSTEM_SOURCES}
    COMMENT "Copying game system libraries to binary directory"
)

# Create executable if this is built as standalone
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Find required dependencies when building standalone
    find_package(raylib CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    
    # Check if main.cpp exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
        add_executable(${RTYPE_PROJECT_NAME}_exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
        target_link_libraries(${RTYPE_PROJECT_NAME}_exe PRIVATE ${RTYPE_PROJECT_NAME})
        set_target_properties(${RTYPE_PROJECT_NAME}_exe PROPERTIES
            OUTPUT_NAME "r-type_client"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
    endif()
endif()

# Export the library for parent projects
set(RTYPE_LIBRARIES ${RTYPE_PROJECT_NAME} PARENT_SCOPE)
set(RTYPE_INCLUDE_DIRS ${RTYPE_INCLUDE_DIRS} PARENT_SCOPE)

# Print configuration info
message(STATUS "RType Game Configuration:")
message(STATUS "  Sources found: ${RTYPE_SOURCES}")
message(STATUS "  Game Systems found: ${GAME_SYSTEM_SOURCES}")
message(STATUS "  Include directories: ${RTYPE_INCLUDE_DIRS}")
message(STATUS "  Building as: ${RTYPE_PROJECT_NAME}")