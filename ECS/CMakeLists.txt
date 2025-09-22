cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ECS_Test VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files for the ecs_test executable
set(ECS_TEST_SOURCES
    main.cpp
    src/registry.cpp
    src/systems.cpp
)

# Source files for the libcomponents.so shared library
set(LIB_COMPONENTS_SOURCES
    src/components/library.cpp
)

# Add the executable
add_executable(ecs_test ${ECS_TEST_SOURCES})

# Add the shared library
add_library(components SHARED ${LIB_COMPONENTS_SOURCES})
set_target_properties(components PROPERTIES OUTPUT_NAME "components")

# Set the runtime path for the executable to find the shared library
set_target_properties(ecs_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ecs_test PROPERTIES INSTALL_RPATH "${CMAKE_BINARY_DIR}")

# Find and link SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(ecs_test sfml-graphics sfml-window sfml-system dl)

# Link the shared library to the executable
add_dependencies(ecs_test components)