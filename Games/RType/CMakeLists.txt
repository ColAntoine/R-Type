cmake_minimum_required(VERSION 3.21)

set(RTYPE_PROJECT_NAME "RType")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

file(GLOB_RECURSE RTYPE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE GAME_SYSTEM_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Entity/Systems/*/*.cpp"
)

list(REMOVE_ITEM RTYPE_SOURCES ${GAME_SYSTEM_SOURCES})
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/main.cpp$")

file(GLOB_RECURSE ALL_COMPONENT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Entity/Components/**/*.cpp"
)

file(GLOB_RECURSE ECS_COMPONENT_SOURCES 
    "${CMAKE_SOURCE_DIR}/ECS/src/components/*.cpp"
    "${CMAKE_SOURCE_DIR}/ECS/src/registry.cpp"
    "${CMAKE_SOURCE_DIR}/ECS/src/dlloader.cpp"
    "${CMAKE_SOURCE_DIR}/ECS/src/library.cpp"
)

set(ALL_REQUIRED_SOURCES ${ALL_COMPONENT_SOURCES} ${ECS_COMPONENT_SOURCES})

file(GLOB_RECURSE RTYPE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER GAME_SYSTEM_SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER ALL_REQUIRED_SOURCES EXCLUDE REGEX ".*/test/.*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RTYPE_SOURCES} ${RTYPE_HEADERS})

set(RTYPE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/States
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Systems
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity/Components
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_SOURCE_DIR}/ECS/include
    ${CMAKE_SOURCE_DIR}/ECS/src
)

add_library(${RTYPE_PROJECT_NAME} STATIC ${RTYPE_SOURCES})

target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC
    ${RTYPE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/Network/include
)

target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC
    ECS
    raylib
    glfw
)

target_compile_features(${RTYPE_PROJECT_NAME} PUBLIC cxx_std_20)

set_target_properties(${RTYPE_PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE ON
)

target_compile_options(${RTYPE_PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

foreach(SYSTEM_FILE ${GAME_SYSTEM_SOURCES})
    get_filename_component(SYSTEM_DIR ${SYSTEM_FILE} DIRECTORY)
    get_filename_component(SYSTEM_DIR_NAME ${SYSTEM_DIR} NAME)
    
    set(GAME_SYSTEM_NAME "game_${SYSTEM_DIR_NAME}")
    
    add_library(${GAME_SYSTEM_NAME} SHARED 
        ${SYSTEM_FILE}
        ${ALL_REQUIRED_SOURCES}
    )
    
    target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
        ${RTYPE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/Network/include
    )
    
    target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC
        raylib
        glfw
    )
    
    if(TARGET ECS)
        target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC ECS)
        target_include_directories(${GAME_SYSTEM_NAME} PUBLIC
            ${CMAKE_SOURCE_DIR}/ECS/include
        )
    endif()
    
    target_compile_features(${GAME_SYSTEM_NAME} PUBLIC cxx_std_20)
    
    set_target_properties(${GAME_SYSTEM_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/systems"
        WINDOWS_EXPORT_ALL_SYMBOLS ON
        POSITION_INDEPENDENT_CODE ON
    )
    
    target_compile_options(${GAME_SYSTEM_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    
    if(TARGET copy_libs)
        add_custom_command(TARGET copy_libs POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                $<TARGET_FILE:${GAME_SYSTEM_NAME}> 
                ${CMAKE_BINARY_DIR}/bin/systems/
        )
    endif()
    
    message(STATUS "Building self-contained game system: ${GAME_SYSTEM_NAME} from ${SYSTEM_FILE}")
endforeach()

set(RTYPE_LIBRARIES ${RTYPE_PROJECT_NAME} PARENT_SCOPE)
set(RTYPE_INCLUDE_DIRS ${RTYPE_INCLUDE_DIRS} PARENT_SCOPE)