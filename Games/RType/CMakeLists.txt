# R-Type Game CMakeLists.txt
cmake_minimum_required(VERSION 3.21)

# Set the project name
set(RTYPE_PROJECT_NAME "RType")

# Collect all source files
file(GLOB_RECURSE RTYPE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Collect all header files  
file(GLOB_RECURSE RTYPE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Filter out test files if any
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/test/.*")

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
message(STATUS "  Include directories: ${RTYPE_INCLUDE_DIRS}")
message(STATUS "  Building as: ${RTYPE_PROJECT_NAME}")